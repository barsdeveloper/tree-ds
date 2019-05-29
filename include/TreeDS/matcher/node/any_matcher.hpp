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
        unsigned current_child = 0;
        auto do_match          = [&](auto&... nodes) {
            if constexpr (sizeof...(nodes) > 0) {
                auto logic = [&](auto& child) {
                    ++current_child;
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
                return (... && logic(nodes));
            } else {
                return true;
            }
        };
        /*
         * Takes as input a child (from `this->children`) and calls the function containing the logic to rematch that
         * child. Why multiple args then if we are only taking one argument? This lambda will be passed to the function
         * `single_apply`, which invokes a callable (`do_rematch`) with an argument which is an element of the tuple at
         * some index. Being that index a runtime entity, we must equipe our function with the ability to accept as
         * argument any element from the tuple or no element at all. This is the reason to wrap the lambda containing
         * the logic into another lambda accepting an arbitrary number of arguments (of any type). All the overloads
         * are relevant at compile time but invariants garantee to never be called at runtime (thus assert(false)).
         */
        auto do_rematch = [&](auto&... args) {
            if constexpr (sizeof...(args) == 1) {
                // Logic to rematch the node
                auto logic = [&](auto& child) -> bool {
                    target_it
                        = policy::pre_order()
                              .get_instance(static_cast<node_t*>(child.target_node), navigator, allocator)
                              .increment();
                    // If the successive target node is the one where the next child started (and failed) its match attempt
                    if (target_it.get_current_node() == match_attempt_begin[child.get_index() + 1]) {
                        // Then his child failed to rematch so as to leave the next child with a potentially positive attempt
                        return false;
                    }
                    return do_match(child);
                };
                return logic(args...);
            } else {
                assert(false); // This branch should never be traversed at runtime.
                return false;
            }
        };
        while (!back_apply(do_match, this->children, current_child)) {
            --current_child; // Becuse do_match increments it always
            while (current_child >= 0 && !single_apply(do_rematch, this->children, current_child)) {
                --current_child;
            }
        }
        return current_child == this->children_count();
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
