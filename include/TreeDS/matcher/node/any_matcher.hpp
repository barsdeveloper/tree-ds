#pragma once

#include <vector>

#include <TreeDS/matcher/node/matcher.hpp>
#include <TreeDS/matcher/pattern.hpp>
#include <TreeDS/matcher/value/true_matcher.hpp>
#include <TreeDS/node/navigator/generative_navigator.hpp>
#include <TreeDS/node/navigator/node_pred_navigator.hpp>
#include <TreeDS/node/struct_node.hpp>
#include <TreeDS/policy/breadth_first.hpp>
#include <TreeDS/policy/pre_order.hpp>

namespace md {

enum class quantifier {
    DEFAULT,
    RELUCTANT,
    GREEDY,
    POSSESSIVE
};

template <quantifier Quantifier, typename ValueMatcher, typename... Children>
class any_matcher : public matcher<any_matcher<Quantifier, ValueMatcher, Children...>, ValueMatcher, Children...> {

    /*   ---   FRIENDS   ---   */
    template <typename, typename, typename...>
    friend class matcher;
    friend class pattern<any_matcher>;
    template <typename VM>
    friend any_matcher<quantifier::DEFAULT, VM> star(const VM&);

    /*   ---   ATTRIBUTES   ---   */
    public:
    static constexpr matcher_info_t info {
        // It matches null only if all its children do so
        (... && Children::info.matches_null),
        // It is reluctant if it has that quantifier
        Quantifier == quantifier::RELUCTANT};

    /*   ---   CONSTRUCTORS   ---   */
    using matcher<any_matcher, ValueMatcher, Children...>::matcher;

    /*   ---   METHODS   ---   */
    private:
    template <typename NodeAllocator, typename CheckNode>
    static void keep_assigning_children(
        allocator_value_type<NodeAllocator>& target,
        const allocator_value_type<NodeAllocator>& reference,
        NodeAllocator& allocator,
        CheckNode&& check_function) {
        using node_t        = allocator_value_type<NodeAllocator>;
        const node_t* child = reference.get_first_child();
        while (child != nullptr) {
            if (!check_function(*child)) {
                continue;
            }
            node_t* new_target = target.assign_child_like(allocate(allocator, child->get_value()), *child);
            keep_assigning_children(*new_target, *child, allocator, check_function);
            child = child->get_next_sibling();
        }
    }

    public:
    template <typename NodeAllocator>
    bool match_node_impl(allocator_value_type<NodeAllocator>& node, NodeAllocator& allocator) {
        if (!this->match_value(node.get_value())) {
            return false;
        }
        using node_t        = allocator_value_type<NodeAllocator>;
        node_t* subtree_cut = nullptr;
        auto predicate      = [&](node_t& n) { // Stop when finding a non-matching node
            if (!n.get_parent()) {
                return this->match_value(n.get_value());
            }
            return n.get_parent() != subtree_cut && this->match_value(n.get_parent()->get_value());
        };
        node_pred_navigator<node_t*, decltype(predicate), true> navigator(&node, predicate, true);
        // Each children has a pointer to the node where it started its match attempt, the last element is nullptr
        std::array<node_t*, any_matcher::children_count() + 1> match_attempt_begin = {nullptr};
        detail::pre_order_impl target_it(policy::pre_order().get_instance(&node, navigator, allocator));
        auto do_match = [&](auto& child) -> bool {
            node_t* current                        = target_it.get_current_node();
            match_attempt_begin[child.get_index()] = current;
            if (current == nullptr) {
                return child.info.matches_null;
            }
            while (current) {
                if (child.match_node(current, allocator)) {
                    subtree_cut = current;
                    target_it.increment();
                    return true;
                }
                target_it.increment();
                current = target_it.get_current_node();
            }
            return false;
        };
        auto do_rematch = [&](auto& child) -> bool {
            subtree_cut = nullptr;
            if (!child.get_node(allocator)) {
                return false;
            }
            target_it
                = policy::pre_order()
                      .get_instance(child.get_node(allocator), navigator, allocator)
                      .go_depth_first_ramification();
            // If the successive target node is the one where the next child started (and failed) its match attempt
            if (target_it.get_current_node() == nullptr) {
                // This child failed to rematch so as to leave the next child with a potentially positive attempt
                return false;
            } else {
                subtree_cut = target_it.get_current_node();
            }
            return do_match(child);
        };
        return this->match_children(do_match, do_rematch);
    }

    template <typename NodeAllocator>
    unique_node_ptr<NodeAllocator> result_impl(NodeAllocator& allocator) {
        using node_t = allocator_value_type<NodeAllocator>;
        unique_node_ptr<NodeAllocator> result;
        if constexpr (any_matcher::children_count() == 0) {
            // Doesn't have children
            switch (Quantifier) {
            case quantifier::RELUCTANT:
                result = nullptr;
                break;
            case quantifier::GREEDY:
                result = this->clone_node(allocator);
                any_matcher::keep_assigning_children(
                    *result,
                    *static_cast<node_t*>(this->target_node),
                    allocator,
                    [this](const node_t& check) {
                        return this->match_value(check.get_value());
                    });
                break;
            case quantifier::POSSESSIVE:
                break;
            default:
                result = this->clone_node(allocator);
            }
        } else {
            // Has children
            if constexpr (Quantifier == quantifier::RELUCTANT) {
                auto& first_child = std::get<0>(this->children);
                if (sizeof...(Children) == 1 && first_child.get_node(allocator) == this->get_node(allocator)) {
                    result = first_child.clone_node(allocator);
                } else {
                    result = this->clone_node(allocator);
                    std::unordered_map<node_t*, node_t*> cloned_nodes;
                    cloned_nodes.insert({this->get_node(allocator), result.get()});
                    auto attach_child = [&](auto& child) {
                        std::pair<node_t*, unique_node_ptr<NodeAllocator>> child_head(
                            child.get_node(allocator),    // same target
                            child.clone_node(allocator)); // cloned target
                        auto it = cloned_nodes.find(child_head.first->get_parent());
                        while (it == cloned_nodes.end()) { // while parent does not exist in cloned_nodes
                            cloned_nodes.insert({child_head.first, child_head.second.get()});
                            child_head = {
                                child_head.first->get_parent(),
                                child_head.second.release()->allocate_assign_parent(allocator, *child_head.first)};
                            it = cloned_nodes.find(child_head.first->get_parent());
                        }
                        it->second->assign_child_like(std::move(child_head.second), *child_head.first);
                    };
                    std::apply(
                        [&](auto&... child) {
                            (..., attach_child(child));
                        },
                        this->children);
                }
            } else if constexpr (Quantifier == quantifier::DEFAULT) {
                std::array<node_t*, any_matcher::children_count()> childrens_nodes;
                auto search_start = childrens_nodes.begin();
                std::apply(
                    [&](auto&... child) {
                        childrens_nodes = {child.get_node(allocator)...};
                    },
                    this->children);
                result = this->clone_node(allocator);
                multiple_node_pointer roots(this->get_node(allocator), result.get());
                // The iterator will use this predicate to traverse the tree. return false => node is not cloned
                auto check_target = [&](auto& multi_node_ptr) {
                    if (search_start == childrens_nodes.end()) {
                        return false;
                    }
                    auto position = std::find(search_start, childrens_nodes.end(), multi_node_ptr.get_master_ptr());
                    if (position != childrens_nodes.end()) {
                        // If we reached a node that is matched by one of the children
                        apply_at_index(
                            [&](auto& child) {
                                // Assign the result of the child to the generated subtree
                                multi_node_ptr.assign_pointer(1, child.result(allocator).release());
                            },
                            this->children,
                            position - childrens_nodes.begin());
                        std::iter_swap(search_start, position);
                        ++search_start;
                        return true;
                    }
                    return this->match_value(multi_node_ptr->get_value());
                };
                generative_navigator nav(allocator, roots, check_target, true);
                detail::breadth_first_impl iterator(policy::breadth_first().get_instance(roots, nav, allocator));
                while (iterator.get_current_node()) {
                    iterator.increment();
                }
            } else {
            }
        }
        return std::move(result);
    }

    template <typename... Nodes>
    constexpr any_matcher<Quantifier, ValueMatcher, Nodes...> replace_children(Nodes&... nodes) const {
        return {this->value, nodes...};
    }
};

template <quantifier Quantifier = quantifier::DEFAULT, typename ValueMatcher>
any_matcher<Quantifier, ValueMatcher> star(const ValueMatcher& value_matcher) {
    return {value_matcher};
}

template <quantifier Quantifier = quantifier::DEFAULT>
any_matcher<Quantifier, true_matcher> star() {
    return {true_matcher()};
}

} // namespace md
