#pragma once

#include <any>
#include <list>
#include <numeric> // std::iota
#include <tuple>   // std::apply
#include <vector>

#include <TreeDS/matcher/node/matcher.hpp>
#include <TreeDS/matcher/pattern.hpp>
#include <TreeDS/matcher/value/true_matcher.hpp>
#include <TreeDS/node/struct_node.hpp>

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
    /// @brief Horizontal cut of the tree where the non matched region starts. Children will start matching from there.
    std::vector<void*> frontier {};

    /*   ---   CONSTRUCTORS   ---   */
    using matcher<any_matcher, ValueMatcher, Children...>::matcher;

    /*   ---   METHODS   ---   */
    template <typename Node>
    bool match_node_impl(Node& node) {
        if (this->children_count() == 1u) {
            if (std::get<0>(this->children).match_node(node)) {
                return true;
            }
        }
        match_subnode(node.get_first_child());
        return false;
    }

    template <typename NodeAllocator>
    unique_node_ptr<NodeAllocator> get_matched_node_impl(NodeAllocator&& allocator) {
        using node_t     = typename NodeAllocator::value_type;
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
        NodeAllocator& allocator,
        typename decltype(any_matcher::frontier)::const_iterator& frontier_it) {
        if (node == nullptr) {
            return {};
        }
        using node_t          = typename NodeAllocator::value_type;
        node_t* current_child = &node;
        do {
            if (*frontier_it == current_child) {
                ++frontier_it;
                continue;
            }
            node_t* target_child = target->shallow_copy_assign_child(current_child, allocator);
            this->allocate_subtree(target_child, current_child->get_first_child(), allocator, frontier_it);
            current_child = current_child->get_next_sibling();
        } while (current_child != nullptr);
    }
}; // namespace md

template <typename ValueMatcher>
any_matcher<ValueMatcher> star(const ValueMatcher& value_matcher) {
    return {value_matcher};
}

any_matcher<true_matcher> star() {
    return {true_matcher()};
}

} // namespace md
