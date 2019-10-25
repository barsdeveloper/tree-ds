#pragma once

#include <unordered_map>
#include <vector>

#include <TreeDS/matcher/node/matcher.hpp>
#include <TreeDS/matcher/pattern.hpp>
#include <TreeDS/matcher/value/true_matcher.hpp>
#include <TreeDS/node/navigator/generative_navigator.hpp>
#include <TreeDS/node/navigator/node_pred_navigator.hpp>
#include <TreeDS/node/struct_node.hpp>
#include <TreeDS/policy/breadth_first.hpp>
#include <TreeDS/policy/leaves.hpp>
#include <TreeDS/policy/pre_order.hpp>

namespace md {

template <
    quantifier Quantifier,
    typename ValueMatcher,
    typename FirstChild  = detail::empty_t,
    typename NextSibling = detail::empty_t>
class any_matcher : public matcher<
                        any_matcher<Quantifier, ValueMatcher, FirstChild, NextSibling>,
                        ValueMatcher,
                        FirstChild,
                        NextSibling> {

    /*   ---   ATTRIBUTES   ---   */
    public:
    static constexpr matcher_info_t info {
        // It matches null only if all its children do so
        any_matcher::foldl_children_types(
            [](auto&& accumulator, auto&& element) {
                using element_type = typename std::remove_reference_t<decltype(element)>::type;
                return accumulator && element_type::info.matches_null;
            },
            true),
        // This node could match null
        true,
        // It is reluctant if it has that quantifier
        Quantifier == quantifier::RELUCTANT,
        Quantifier == quantifier::POSSESSIVE};
    const void* subtree_cut = nullptr;

    /*   ---   CONSTRUCTORS   ---   */
    using matcher<any_matcher, ValueMatcher, FirstChild, NextSibling>::matcher;

    /*   ---   METHODS   ---   */
    private:
    template <typename NodeAllocator, typename Iterator>
    auto get_match_iterator(NodeAllocator& allocator, Iterator& it) {
        using tree_t     = typename Iterator::tree_type;
        using node_ptr_t = decltype(it.get_raw_node());
        auto predicate   = [this](node_ptr_t node_ptr) {
            // Parent always present (because we start matching from the child of node)
            if constexpr (any_matcher::info.possessive) {
                return this->match_value(node_ptr->get_parent()->get_value());
            } else {
                return node_ptr->get_parent() != this->subtree_cut && this->match_value(node_ptr->get_parent()->get_value());
            }
        };
        using navigator_t = node_pred_navigator<node_ptr_t, decltype(predicate)>;
        navigator_t navigator(it.get_raw_node(), predicate);
        if constexpr (any_matcher::info.possessive) {
            return ++tree_iterator<tree_t, policy::leaves, navigator_t>(
                policy::leaves()
                    .get_instance(static_cast<node_ptr_t>(nullptr), navigator, allocator)
                    .go_first());
        } else {
            return ++tree_iterator<tree_t, policy::pre_order, navigator_t>(
                policy::pre_order().get_instance(it.get_raw_node(), navigator, allocator));
        }
    }

    template <typename NodeAllocator>
    auto get_search_funcion(allocator_value_type<NodeAllocator>&, NodeAllocator& allocator) {
        return [&](auto& it, auto& child) -> bool {
            using node_t    = allocator_value_type<NodeAllocator>;
            node_t* current = it.get_current_node();
            if constexpr (any_matcher::info.possessive) {
                if (current && this->match_value(current->get_value())) {
                    return false;
                }
            }
            if (child.search_node(current, allocator)) {
                subtree_cut = current;
                return true;
            }
            return false;
        };
    }

    template <typename NodeAllocator>
    auto get_backtrack_funcion(NodeAllocator& allocator) {
        if constexpr (any_matcher::info.possessive) {
            /*
             * Possessive any_matcher matches any node it can then tries to match its children just on leaf nodes. It
             * cannot rematch in case of children bad match (a match that leaves the other children without a match)
             * because the leaves constitutes a linear data structure.
             */
            return detail::empty_t();
        } else {
            return [&](auto& matcher, auto& it) -> bool {
                this->subtree_cut = nullptr;
                if (matcher.empty()) {
                    return false; // The matcher didn't match anything so it cannot backtrack
                }
                using it_t       = std::decay_t<decltype(it)>;
                using node_ptr_t = decltype(it.get_raw_node());
                // From the target of this matcher, advance in depth as to leave the next matcher with an unseen target
                auto policy = policy::pre_order()
                                  .get_instance(static_cast<node_ptr_t>(matcher.get_node(allocator)), it.get_navigator(), allocator)
                                  .go_depth_first_ramification();
                it                = it_t(policy);
                this->subtree_cut = nullptr;
                return true;
            };
        }
    }

    template <typename NodeAllocator, typename CheckNode>
    static void keep_assigning_children(
        allocator_value_type<NodeAllocator>& target,
        const allocator_value_type<NodeAllocator>& reference,
        NodeAllocator& allocator,
        CheckNode&& check_function) {
        using node_t        = allocator_value_type<NodeAllocator>;
        const node_t* child = reference.get_first_child();
        while (child) {
            if (!check_function(*child)) {
                continue;
            }
            node_t* new_target = target.assign_child_like(allocate(allocator, child->get_value()), *child);
            keep_assigning_children(*new_target, *child, allocator, check_function);
            child = child->get_next_sibling();
        }
    }

    template <typename NodeAllocator>
    void no_children_set_result(NodeAllocator allocator, unique_ptr_alloc<NodeAllocator>& result) {
        switch (Quantifier) {
        case quantifier::RELUCTANT:
            // Reluctant any_matcher without children matches nothing
            result = nullptr;
            break;
        case quantifier::GREEDY:
        case quantifier::POSSESSIVE:
            // Greedy and possessive any_matcher without children matches everithing they can
            result = this->clone_node(allocator);
            any_matcher::keep_assigning_children(
                *result,
                *this->get_node(allocator),
                allocator,
                [this](const allocator_value_type<NodeAllocator>& check) {
                    return this->match_value(check.get_value());
                });
            break;
        case quantifier::DEFAULT:
        default:
            // Default any_matcher without children matches just a single node
            result = this->clone_node(allocator);
        }
    }

    template <typename NodeAllocator>
    void children_reluctant_set_result(NodeAllocator allocator, unique_ptr_alloc<NodeAllocator>& result) {
        using node_t = allocator_value_type<NodeAllocator>;
        unique_ptr_alloc<NodeAllocator> candidate;
        if (this->did_child_steal_target(candidate, allocator)) {
            result = std::move(candidate);
            return;
        }
        result = this->clone_node(allocator);
        std::unordered_map<node_t*, node_t*> cloned_nodes;
        cloned_nodes[this->get_node(allocator)] = result.get();
        auto attach_child                       = [&](auto& child) {
            std::pair<node_t*, unique_ptr_alloc<NodeAllocator>> child_head(
                child.get_node(allocator),    // child target
                child.clone_node(allocator)); // cloned target
            auto it = cloned_nodes.find(child_head.first->get_parent());
            while (it == cloned_nodes.end()) { // while parent does not exist in cloned_nodes
                cloned_nodes[child_head.first] = child_head.second.get();
                child_head                     = {
                    child_head.first->get_parent(),
                    child_head.second.release()->allocate_assign_parent(allocator, *child_head.first)};
                it = cloned_nodes.find(child_head.first->get_parent());
            }
            it->second->assign_child_like(std::move(child_head.second), *child_head.first);
        };
        this->foldl_children(
            [&](bool, auto& child) {
                attach_child(child);
                return true;
            },
            true);
    }

    template <typename NodeAllocator>
    void children_set_result(NodeAllocator& allocator, unique_ptr_alloc<NodeAllocator>& result) {
        using node_t = allocator_value_type<NodeAllocator>;
        // If a children stole the target
        unique_ptr_alloc<NodeAllocator> candidate;
        if (this->did_child_steal_target(candidate, allocator)) {
            result = std::move(candidate);
            return;
        }
        result = this->clone_node(allocator);
        std::unordered_map<node_t*, unique_ptr_alloc<NodeAllocator>> children_targets;
        children_targets.reserve(this->children());
        int valid_targets = this->foldl_children(
            [&](unsigned accumulated, auto& child) {
                if (!child.empty()) {
                    children_targets[child.get_node(allocator)] = child.result(allocator);
                    ++accumulated;
                }
                return accumulated;
            },
            0);
        using multi_ptr_t = multiple_node_pointer<node_t*, node_t*>;
        multi_ptr_t roots(this->get_node(allocator), result.get());
        // The iterator will use this predicate to traverse the tree
        // !IMPORTANT make multi_ptr a reference because we may need to assign the pointer
        auto check_target = [&](multi_ptr_t& multi_ptr) {
            if (Quantifier == quantifier::DEFAULT && valid_targets == 0) {
                return false;
            }
            auto it = children_targets.find(multi_ptr.get_main_pointer());
            if (it != children_targets.end()) {
                // We found a node that was matched by a child
                // Just assign the pointer it should have, generative_navigator will take care to attach it to the tree
                multi_ptr.assign_pointer(1, it->second.release());
                --valid_targets;
                return true;
            }
            return this->match_value(multi_ptr->get_value());
        };
        generative_navigator nav(roots, check_target, allocator);
        detail::breadth_first_impl iterator(policy::breadth_first().get_instance(roots, nav, allocator));
        while (iterator) {
            iterator.increment();
        }
    }

    public:
    template <typename NodeAllocator, typename Iterator>
    bool search_node_impl(NodeAllocator& allocator, Iterator& it) {
        if (!this->match_value(*it)) {
            // If this matcher does not accepth the node, there is still one possibility: a single child can match it
            if constexpr (any_matcher::child_may_steal_target()) {
                return this->search_node_child(allocator, it.other_policy(policy::fixed()));
            } else {
                return false;
            }
        }
        auto target_it      = this->get_match_iterator(allocator, it);
        using node_t        = allocator_value_type<NodeAllocator>;
        auto do_acknowledge = [this](const node_t* matched) {
            this->subtree_cut = matched;
        };
        auto do_backtrack = this->get_backtrack_funcion(allocator);
        bool result       = this->search_node_child(allocator, target_it, do_acknowledge, do_backtrack);
        if constexpr (!any_matcher::info.possessive && any_matcher::child_may_steal_target()) {
            // Every other match failed, we try the last possibility: one of the children matches this node
            if (!result) {
                return this->search_node_child(allocator, it.other_policy(policy::fixed()));
            }
        }
        return result;
    }

    template <typename NodeAllocator>
    unique_ptr_alloc<NodeAllocator> result_impl(NodeAllocator& allocator) {
        unique_ptr_alloc<NodeAllocator> result = nullptr;
        bool all_children_empty                = this->foldl_children(
            [](bool accumulated, auto child) {
                return accumulated && child.empty();
            },
            true);
        if (all_children_empty) {
            this->no_children_set_result(allocator, result);
        } else if constexpr (any_matcher::children() == 0) {
            this->no_children_set_result(allocator, result);
        } else {
            // Has children
            if constexpr (Quantifier == quantifier::RELUCTANT) {
                this->children_reluctant_set_result(allocator, result);
            } else if constexpr (
                Quantifier == quantifier::DEFAULT
                || Quantifier == quantifier::GREEDY
                || Quantifier == quantifier::POSSESSIVE) {
                this->children_set_result(allocator, result);
            }
        }
        return std::move(result);
    }

    template <typename Child>
    constexpr any_matcher<Quantifier, ValueMatcher, std::remove_reference_t<Child>, NextSibling>
    with_first_child(Child&& child) const {
        return {this->value, child, this->next_sibling};
    }

    template <typename Sibling>
    constexpr any_matcher<Quantifier, ValueMatcher, FirstChild, std::remove_reference_t<Sibling>>
    with_next_sibling(Sibling&& sibling) const {
        return {this->value, this->first_child, sibling};
    }
};

template <quantifier Quantifier = quantifier::DEFAULT, typename ValueMatcher>
any_matcher<Quantifier, ValueMatcher, detail::empty_t>
star(const ValueMatcher& value_matcher) {
    return {value_matcher, detail::empty_t(), detail::empty_t()};
}

template <quantifier Quantifier = quantifier::DEFAULT>
any_matcher<Quantifier, true_matcher, detail::empty_t>
star() {
    return {true_matcher(), detail::empty_t(), detail::empty_t()};
}

} // namespace md
