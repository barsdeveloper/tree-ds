#pragma once

#include <any>
#include <list>
#include <numeric> // std::iota
#include <tuple>   // std::apply
#include <vector>

#include <TreeDS/matcher/node/matcher.hpp>
#include <TreeDS/matcher/pattern.hpp>
#include <TreeDS/matcher/value/true_matcher.hpp>
#include <TreeDS/node/node_pred_navigator.hpp>
#include <TreeDS/node/struct_node.hpp>
#include <TreeDS/policy/pre_order.hpp>

namespace md {

template <typename ValueMatcher, typename... Children>
class any_matcher : public matcher<any_matcher<ValueMatcher, Children...>, ValueMatcher, Children...> {

    /*   ---   FRIENDS   ---   */
    template <typename, typename, typename...>
    friend class matcher;
    friend class pattern<any_matcher>;
    template <typename VM>
    friend any_matcher<VM> star(const VM&);

    /*   ---   ATTRIBUTES   ---   */
    public:
    static constexpr matcher_info_t info {(... && Children::info.matches_null), false};

    /*   ---   CONSTRUCTORS   ---   */
    using matcher<any_matcher, ValueMatcher, Children...>::matcher;

    /*   ---   METHODS   ---   */
    template <typename NodeAllocator>
    bool match_node_impl(allocator_value_type<NodeAllocator>& node, NodeAllocator& allocator) {
        if constexpr (sizeof...(Children) > 0) {
            using node_t = allocator_value_type<NodeAllocator>;
            node_pred_navigator navigator(
                &node,              // Navigate the subtree represented by node
                [this](node_t& n) { // Stop when finding a non-matching node
                    return this->match_value(n);
                },
                true);
            // Each children has a pointer to the node where it started its match attempt, the last one is always nullptr
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
                    if (child.match_node(*current)) {
                        return true;
                    }
                    current = target_it.get_current_node();
                }
                return false;
            };
            auto do_rematch = [&](auto& child) -> bool {
                target_it
                    = policy::pre_order()
                          .get_instance(static_cast<node_t*>(child.target_node), navigator, allocator)
                          .depth_increment();
                // If the successive target node is the one where the next child started (and failed) its match attempt
                if (target_it.get_current_node() == nullptr) {
                    // Then his child failed to rematch so as to leave the next child with a potentially positive attempt
                    return false;
                }
                return do_match(child);
            };
            int current_child;
            for (current_child = 0; current_child < this->children_count(); ++current_child) {
                if (!apply_at_index(do_match, this->children, current_child)) {
                    while (--current_child >= 0) {
                        if (apply_at_index(do_rematch, this->children, current_child)) {
                            break; // We found a child that could rematch and leave the other children new nodes
                        }
                    }
                }
            }
            return current_child == this->children_count();
        } else {
            return true;
        }
    }

    template <typename NodeAllocator>
    unique_node_ptr<NodeAllocator> get_matched_node_impl(NodeAllocator& allocator) {
        using node_t     = allocator_value_type<NodeAllocator>;
        auto frontier_it = this->frontier.cbegin();
        unique_node_ptr<NodeAllocator> root;
        if (this->matched_node != nullptr) {
            root = allocate(allocator, static_cast<node_t*>(this->matched_node)->get_value());
            this->allocate_subtree(root.get(), this->matched_node->get_first_child(), allocator, frontier_it);
        }
        return root;
    }

    template <typename NodeAllocator>
    unique_node_ptr<NodeAllocator> allocate_subtree(
        typename NodeAllocator::value_type& target,
        typename NodeAllocator::value_type* node,
        NodeAllocator& allocator) {
        return nullptr;
    }

    template <typename... Nodes>
    constexpr any_matcher<ValueMatcher, Nodes...> with_children(Nodes&... nodes) const {
        return {this->value, nodes...};
    }
};

template <typename ValueMatcher>
any_matcher<ValueMatcher> star(const ValueMatcher& value_matcher) {
    return {value_matcher};
}

} // namespace md
