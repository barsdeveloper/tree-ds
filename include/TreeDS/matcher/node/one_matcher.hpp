#pragma once

#include <functional> // std::reference_wrapper
#include <optional>

#include <TreeDS/allocator_utility.hpp>
#include <TreeDS/matcher/node/matcher.hpp>
#include <TreeDS/matcher/pattern.hpp>
#include <TreeDS/matcher/value/true_matcher.hpp>

namespace md {

template <typename ValueMatcher, typename... Children>
class one_matcher : public matcher<one_matcher<ValueMatcher, Children...>, ValueMatcher, Children...> {

    /*   ---   FRIENDS   ---   */
    template <typename, typename, typename...>
    friend class matcher;

    /*   ---   ATTRIBUTES   ---   */
    public:
    static constexpr matcher_info_t info {false, true};

    /*   ---   CONSTRUCTOR   ---   */
    using matcher<one_matcher, ValueMatcher, Children...>::matcher;

    /*   ---   METHODS   ---   */
    template <typename NodeAllocator>
    bool match_node_impl(allocator_value_type<NodeAllocator>& node, NodeAllocator& allocator) {
        if (!this->match_value(node.get_value())) {
            return false;
        }
        // Match children of the pattern
        auto node_supplier = one_matcher::get_children_supplier(node);
        return this->match_children(node_supplier, allocator);
    }

    template <typename NodeAllocator>
    unique_node_ptr<NodeAllocator> get_matched_node_impl(NodeAllocator& allocator) {
        unique_node_ptr<NodeAllocator> result = allocate(
            allocator,
            static_cast<allocator_value_type<NodeAllocator>*>(this->target_node)->get_value());
        this->attach_matched_children(
            [&]() {
                return result.get();
            },
            allocator);
        return std::move(result);
    }

    template <typename NodeAllocator>
    void attach_matched_impl(allocator_value_type<NodeAllocator>& target, NodeAllocator& allocator) {
        target.shallow_copy_assign_child(
            *static_cast<allocator_value_type<NodeAllocator>*>(this->target_node),
            std::forward<NodeAllocator>(allocator));
    }

    template <typename... Nodes>
    constexpr one_matcher<ValueMatcher, Nodes...> with_children(Nodes&... nodes) const {
        return {this->value, nodes...};
    }
};

template <typename ValueMatcher>
one_matcher<ValueMatcher> one(const ValueMatcher& value_matcher) {
    return {value_matcher};
}

} // namespace md
