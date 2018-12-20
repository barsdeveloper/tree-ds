#pragma once

#include <memory> // std::allocator

#include <TreeDS/binary_tree.hpp>
#include <TreeDS/node/nary_node.hpp>
#include <TreeDS/policy/pre_order.hpp>
#include <TreeDS/tree_base.hpp>

namespace md::nary {

/**
 * @brief A type of tree having nodes {@link nary::node}
 *
 * @tparam T the type of value hold by this tree
 * @tparam Policy default traversal algorithm
 * @tparam Allocator the allocater used to allocate nodes
 * @relates tree
 */
template <
    typename T,
    typename Policy    = pre_order,
    typename Allocator = std::allocator<T>>
class tree : public md::tree_base<T, nary::node<T>, Policy, Allocator> {
    using super = md::tree_base<T, nary::node<T>, Policy, Allocator>;

    // Inherit constructors from parent class
    using md::tree_base<T, nary::node<T>, Policy, Allocator>::tree_base;

    public:
    /// @brief Construct from {@link #binary::tree}
    template <typename OtherPolicy, typename OtherAllocator>
    tree(const binary::tree<T, OtherPolicy, OtherAllocator>& other) :
            md::tree_base<T, nary::node<T>, Policy, Allocator>(
                other.get_root()
                    ? allocate(this->allocator, *other.get_root(), this->allocator).release()
                    : nullptr,
                static_cast<std::size_t>(other.size())) {
        static_assert(
            std::is_copy_constructible_v<T>,
            "Tried to construct an nary::tree from a binary::tree containing a non copyable type.");
    }

    // Import the overloads of the operator= into the current class (that would be shadowed otherwise)
    using md::tree_base<T, nary::node<T>, Policy, Allocator>::operator=;

    template <typename OtherPolicy, typename OtherAllocator>
    tree& operator=(const binary::tree<T, OtherPolicy, OtherAllocator>& other) {
        static_assert(
            std::is_copy_assignable_v<T>,
            "Tried to assign to an nary::tree a binary::tree containing a non copyable type.");
        this->assign(
            other.get_root() != nullptr
                ? allocate(this->allocator, *other.get_root(), this->allocator).release()
                : nullptr,
            other.size());
        return *this;
    }

    template <typename OtherPolicy, typename OtherAllocator>
    tree& operator=(binary::tree<T, OtherPolicy, OtherAllocator>&& other) {
        this->assign(
            other.get_root(),
            other.size());
        other.emptify();
        return *this;
    }

    // Import the overloads of the operator== into the current class (would be shadowed otherwise)
    using md::tree_base<T, nary::node<T>, Policy, Allocator>::operator==;

    template <typename OtherPolicy, typename OtherAllocator>
    bool operator==(const binary::tree<T, OtherPolicy, OtherAllocator>& other) const {
        // 1. Test if different size_value
        if (this->size() != other.size()) {
            return false;
        }
        // 2. Test if one between this or other has a root that is set while the other doesn't.
        if ((this->get_root() == nullptr) != (other.get_root() == nullptr)) {
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
    typename Allocator1,
    typename Allocator2>
bool operator==(
    const binary::tree<T, Policy1, Allocator1>& lhs,
    const nary::tree<T, Policy2, Allocator2>& rhs) {
    return rhs.operator==(lhs);
}

template <
    typename T,
    typename Policy1,
    typename Policy2,
    typename Allocator1,
    typename Allocator2>
bool operator!=(
    const nary::tree<T, Policy2, Allocator2>& lhs,
    const binary::tree<T, Policy1, Allocator1>& rhs) {
    return !lhs.operator==(rhs);
}

template <
    typename T,
    typename Policy1,
    typename Policy2,
    typename Allocator1,
    typename Allocator2>
bool operator!=(
    const binary::tree<T, Policy1, Allocator1>& lhs,
    const nary::tree<T, Policy2, Allocator2>& rhs) {
    return !rhs.operator==(lhs);
}

} // namespace md::nary
