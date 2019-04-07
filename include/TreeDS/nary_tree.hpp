#pragma once

#include <memory> // std::allocator

#include <TreeDS/binary_tree.hpp>
#include <TreeDS/node/nary_node.hpp>
#include <TreeDS/policy/pre_order.hpp>
#include <TreeDS/tree.hpp>

namespace md {

/**
 * @brief A type of tree having nodes {@link nary_node}
 *
 * @tparam T the type of value hold by this tree
 * @tparam Policy default traversal algorithm
 * @tparam Allocator the allocater used to allocate nodes
 * @relates tree
 */
template <
    typename T,
    typename Policy    = default_policy,
    typename Allocator = std::allocator<T>>
class nary_tree : public tree<T, nary_node<T>, Policy, Allocator> {
    using super = tree<T, nary_node<T>, Policy, Allocator>;

    // Inherit constructors from parent class
    using tree<T, nary_node<T>, Policy, Allocator>::tree;

    public:
    /// @brief Construct from {@link #binary_tree}
    template <typename OtherPolicy>
    nary_tree(const binary_tree<T, OtherPolicy, Allocator>& other) :
            tree<T, nary_node<T>, Policy, Allocator>(
                other.get_root()
                    ? allocate(this->allocator, *other.get_root(), this->allocator).release()
                    : nullptr,
                other.size(),
                other.arity()) {
        static_assert(
            std::is_copy_constructible_v<T>,
            "Tried to construct an nary_tree from a binary_tree containing a non copyable type.");
    }

    // Import the overloads of the operator= into the current class (that would be shadowed otherwise)
    using tree<T, nary_node<T>, Policy, Allocator>::operator=;

    template <typename OtherPolicy>
    nary_tree& operator=(const binary_tree<T, OtherPolicy, Allocator>& other) {
        static_assert(
            std::is_copy_assignable_v<T>,
            "Tried to assign to an nary_tree a binary_tree containing a non copyable type.");
        this->assign(
            other.get_root() != nullptr
                ? allocate(this->allocator, *other.get_root(), this->allocator).release()
                : nullptr,
            other.size(),
            other.arity());
        return *this;
    }

    // Import the overloads of the operator== into the current class (would be shadowed otherwise)
    using tree<T, nary_node<T>, Policy, Allocator>::operator==;

    template <typename OtherPolicy>
    bool operator==(const binary_tree<T, OtherPolicy, Allocator>& other) const {
        // Test if different size_value
        if (this->empty() != other.empty()
            || this->size() != other.size()
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
    const binary_tree<T, Policy1, Allocator>& lhs,
    const nary_tree<T, Policy2, Allocator>& rhs) {
    return rhs.operator==(lhs);
}

template <
    typename T,
    typename Policy1,
    typename Policy2,
    typename Allocator>
bool operator!=(
    const nary_tree<T, Policy1, Allocator>& lhs,
    const binary_tree<T, Policy2, Allocator>& rhs) {
    return !lhs.operator==(rhs);
}

template <
    typename T,
    typename Policy1,
    typename Policy2,
    typename Allocator>
bool operator!=(
    const binary_tree<T, Policy1, Allocator>& lhs,
    const nary_tree<T, Policy2, Allocator>& rhs) {
    return !rhs.operator==(lhs);
}

} // namespace md
