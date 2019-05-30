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
    constexpr any_matcher<Quantifier, ValueMatcher, Nodes...> with_children(Nodes&... nodes) const {
        return {this->value, nodes...};
    }
};

template <quantifier Quantifier = quantifier::DEFAULT, typename ValueMatcher>
any_matcher<Quantifier, ValueMatcher> star(const ValueMatcher& value_matcher) {
    return {value_matcher};
}

} // namespace md
