#pragma once

#include <memory> // std::allocator

#include <TreeDS/basic_tree.hpp>
#include <TreeDS/policy/pre_order.hpp>
#include <TreeDS/viewer/binary_tree_view.hpp>
#include <TreeDS/viewer/tree_view.hpp>

namespace md {

template <
    typename T,
    typename Policy    = default_policy,
    typename Allocator = std::allocator<T>>
class nary_tree_view : public tree_view<T, nary_node<T>, Policy, Allocator> {

    using tree_view<T, nary_node<T>, Policy, Allocator>::tree_view;

    public:
    DECLARE_TYPES(T, const nary_node<T>, Policy, Allocator)

    // Import the overloads of the operator== into the current class (would be shadowed otherwise)
    using tree_view<T, nary_node<T>, Policy, Allocator>::operator==;

    template <typename OtherPolicy>
    bool operator==(const binary_tree_view<T, OtherPolicy, Allocator>& other) const {
        // Test if different size_value
        if (this->size() != other.size()
            || this->arity() != other.arity()) {
            return false;
        }
        // At the end is either null (both) or same as the other.
        return this->get_root() == nullptr || *this->get_root() == *other.get_root();
    }
};

template <
    typename T,
    typename Policy1,
    typename Policy2,
    typename Allocator>
bool operator==(
    const binary_tree_view<T, Policy1, Allocator>& lhs,
    const nary_tree_view<T, Policy2, Allocator>& rhs) {
    return rhs.operator==(lhs);
}

template <
    typename T,
    typename Policy1,
    typename Policy2,
    typename Allocator>
bool operator!=(
    const nary_tree_view<T, Policy1, Allocator>& lhs,
    const binary_tree_view<T, Policy2, Allocator>& rhs) {
    return !lhs.operator==(rhs);
}

template <
    typename T,
    typename Policy1,
    typename Policy2,
    typename Allocator>
bool operator!=(
    const binary_tree_view<T, Policy1, Allocator>& lhs,
    const nary_tree_view<T, Policy2, Allocator>& rhs) {
    return !rhs.operator==(lhs);
}

} // namespace md
