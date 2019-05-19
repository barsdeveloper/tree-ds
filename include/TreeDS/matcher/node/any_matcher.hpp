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
class any_matcher : public matcher<any_matcher, ValueMatcher, Children...> {

    /*   ---   FRIENDS   ---   */
    template <template <typename, typename...> class, typename, typename...>
    friend class matcher;
    friend class pattern<any_matcher>;
    template <typename VM>
    friend any_matcher<VM> star(const VM&);
    friend any_matcher<true_matcher> star();

    /*   ---   TYPES   ---   */
    public:
    using super = matcher<any_matcher, ValueMatcher, Children...>;
    using typename super::captures_t;

    /*   ---   ATTRIBUTES   ---   */
    protected:
    static constexpr matcher_info_t info {true, false};

    /*   ---   CONSTRUCTORS   ---   */
    using matcher<any_matcher, ValueMatcher, Children...>::matcher;

    /*   ---   METHODS   ---   */
    template <typename NodeAllocator>
    bool match_node_impl(allocator_value_type<NodeAllocator>& node, NodeAllocator& allocator) {
        using node_t = allocator_value_type<NodeAllocator>;
        node_pred_navigator navigator(
            &node,              // Navigate the subtree represented by node.
            [this](node_t& n) { // Stop when finding a non-matching node.
                return this->match_value(n);
            });
        detail::pre_order_impl<node_t, decltype(navigator), NodeAllocator> it(&node, navigator, allocator);
        // TODO get nodes and shallow match agains children...
        // Then deep match the children.
        return false;
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
};

template <typename ValueMatcher>
any_matcher<ValueMatcher> star(const ValueMatcher& value_matcher) {
    return {value_matcher};
}

} // namespace md
