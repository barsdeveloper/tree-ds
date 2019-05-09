#pragma once

#include <functional> // std::reference_wrapper
#include <optional>

#include <TreeDS/allocator_utility.hpp>
#include <TreeDS/matcher/node/matcher.hpp>
#include <TreeDS/matcher/pattern.hpp>
#include <TreeDS/matcher/value/true_matcher.hpp>

namespace md {

template <typename ValueMatcher, typename... Children>
class one_matcher : public matcher<one_matcher, ValueMatcher, Children...> {

    /*   ---   FRIENDS   ---   */
    template <template <typename, typename...> class, typename, typename...>
    friend class matcher;
    friend class pattern<one_matcher>;
    template <typename VM>
    friend one_matcher<VM> one(const VM&);

    /*   ---   TYPES   ---   */
    public:
    using super = matcher<one_matcher, ValueMatcher, Children...>;
    using typename super::captures_t;

    /*   ---   ATTRIBUTES   ---   */
    public:
    static constexpr matcher_info_t info {false, true};

    /*   ---   CONSTRUCTOR   ---   */
    using matcher<one_matcher, ValueMatcher, Children...>::matcher;

    /*   ---   METHODS   ---   */
    template <typename Node>
    bool match_node_impl(Node& node) {
        if (!this->match_value(node.get_value())) {
            return false;
        }
        // Match children of the pattern
        auto node_supplier = one_matcher::get_children_supplier(node);
        return this->match_children(node_supplier);
    }

    template <typename NodeAllocator>
    void copy_matched_subtree_impl(typename NodeAllocator::value_type& target, NodeAllocator&& allocator) {
        target.shallow_copy_assign_child(
            *static_cast<typename NodeAllocator::value_type*>(this->matched_node),
            std::forward<NodeAllocator>(allocator));
    }

    template <typename NodeAllocator>
    unique_node_ptr<NodeAllocator> get_matched_node_impl(NodeAllocator& allocator) {
        return allocate(
            allocator,
            static_cast<allocator_value_type<NodeAllocator>*>(this->target_node)->get_value());
    }
};

template <typename ValueMatcher>
one_matcher<ValueMatcher> one(const ValueMatcher& value_matcher) {
    return {value_matcher};
}

} // namespace md
