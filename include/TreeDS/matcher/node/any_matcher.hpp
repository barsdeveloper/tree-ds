#pragma once

#include <TreeDS/matcher/node/matcher.hpp>
#include <TreeDS/matcher/pattern.hpp>
#include <TreeDS/matcher/value/true_matcher.hpp>
#include <TreeDS/node/node_pred_navigator.hpp>
#include <TreeDS/node/struct_node.hpp>
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
            node_t* new_target = target.allocate_assign_child(*child, allocator);
            keep_assigning_children(*new_target, *child, allocator, check_function);
            child = child->get_next_sibling();
        }
    }

    public:
    template <typename NodeAllocator>
    bool match_node_impl(allocator_value_type<NodeAllocator>& node, NodeAllocator& allocator) {
        using node_t = allocator_value_type<NodeAllocator>;
        node_pred_navigator navigator(
            &node,              // Navigate the subtree represented by node
            [this](node_t& n) { // Stop when finding a non-matching node
                return this->match_value(n);
            },
            true);
        // Each children has a pointer to the node where it started its match attempt, the last element is nullptr
        std::array<node_t*, sizeof...(Children) + 1> match_attempt_begin = {};
        detail::pre_order_impl target_it(policy::pre_order().get_instance(&node, navigator, allocator));
        auto do_match = [&](auto& child) -> bool {
            node_t* current                        = target_it.get_current_node();
            match_attempt_begin[child.get_index()] = current;
            if (current == nullptr) {
                return child.info.matches_null;
            }
            while (current) {
                target_it.increment();
                if (child.match_node(current, allocator)) {
                    return true;
                }
                current = target_it.get_current_node();
            }
            return false;
        };
        auto do_rematch = [&](auto& child) -> bool {
            target_it
                = policy::pre_order()
                      .get_instance(child.get_node(allocator), navigator, allocator)
                      .go_depth_first_ramification();
            // If the successive target node is the one where the next child started (and failed) its match attempt
            if (target_it.get_current_node() == nullptr) {
                // This child failed to rematch so as to leave the next child with a potentially positive attempt
                return false;
            }
            return do_match(child);
        };
        return this->match_children(do_match, do_rematch);
    }

    template <typename NodeAllocator>
    unique_node_ptr<NodeAllocator> result_impl(NodeAllocator& allocator) {
        if constexpr (any_matcher::children_count() == 0) {
            if constexpr (Quantifier == quantifier::RELUCTANT) {
                return nullptr;
            } else if constexpr (Quantifier == quantifier::DEFAULT) {
                return this->clone_target_node(allocator);
            } else {
                using node_t = allocator_value_type<NodeAllocator>;
                unique_node_ptr<NodeAllocator> node(this->clone_target_node(allocator));
                any_matcher::keep_assigning_children(
                    *node,
                    *static_cast<node_t*>(this->target_node),
                    allocator,
                    [this](const node_t& check) {
                        return this->match_value(check.get_value());
                    });
            }
        } else {
            using node_t = allocator_value_type<NodeAllocator>;
            if constexpr (Quantifier == quantifier::RELUCTANT) {
                node_t* top_node                      = this->get_node(allocator);
                node_t* head                          = std::get<0>(this->children).get_node(allocator);
                unique_node_ptr<NodeAllocator> result = std::get<0>(this->children).result(allocator);
                std::unordered_map<node_t*, node_t*> cloned_nodes;
                // Clone first child branch
                while (head != top_node) {
                    result = result.release()->allocate_assign_parent(allocator, *head);
                    head   = head->get_parent();
                    if constexpr (any_matcher::children_count() > 1) {
                        cloned_nodes.insert({head, result.get()});
                    }
                }
                if constexpr (any_matcher::children_count() > 1) {
                    // Clone and attach the other children branches
                    auto attach_child = [&](auto& child) {
                        std::pair<node_t*, unique_node_ptr<NodeAllocator>> child_head(
                            child.get_referred_node(allocator),
                            child.clone_referred_node(allocator));
                        for (
                            auto parent_it = cloned_nodes.find(child_head.first->get_parent());
                            parent_it == cloned_nodes.end();
                            parent_it = cloned_nodes.find(child.first->get_parent())) {
                            child_head = {
                                child_head.first->get_parent(),
                                child_head.second->allocate_assign_parent()};
                        }
                    };
                    for (std::size_t current_child = 1; current_child < any_matcher::children_count(); ++current_child) {
                        apply_at_index(attach_child, this->children, current_child);
                    }
                }
                return std::move(result);
            } else if constexpr (Quantifier == quantifier::DEFAULT) {
            } else {
            }
        }
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

} // namespace md
