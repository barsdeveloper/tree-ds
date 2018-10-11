#pragma once

#include <memory> // std::allocator

#include <TreeDS/binary_tree.hpp>
#include <TreeDS/node/nary_node.hpp>
#include <TreeDS/traversal_policy/pre_order.hpp>
#include <TreeDS/tree.hpp>

namespace ds {

/**
 * @brief A type of tree having nodes {@link nary_node}
 *
 * @tparam T the type of value hold by this tree
 * @tparam Algorithm default traverasl algorithm
 * @tparam Allocator the allocater used to allocate nodes
 * @relates tree
 */
template <
    typename T,
    typename Algorithm = pre_order,
    typename Allocator = std::allocator<T>>
class nary_tree : public tree<T, nary_node<T>, Algorithm, Allocator> {
    using super = tree<T, nary_node<T>, Algorithm, Allocator>;
    using tree<T, nary_node<T>, Algorithm, Allocator>::tree;

    public:
    template <typename OtherAlgorithm, typename OtherAllocator>
    nary_tree(const binary_tree<T, OtherAlgorithm, OtherAllocator>& other) :
            tree<T, nary_node<T>, Algorithm, Allocator>(
                other.get_root(),
                static_cast<std::size_t>(other.size())) {
    }

    // Import the overloads of the operator== into the current class
    using tree<T, nary_node<T>, Algorithm, Allocator>::operator==;

    template <typename OtherAlgorithm, typename OtherAllocator>
    bool operator==(const binary_tree<T, OtherAlgorithm, OtherAllocator>& other) const {
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
    typename Algorithm1,
    typename Algorithm2,
    typename Allocator1,
    typename Allocator2>
bool operator==(
    const binary_tree<T, Algorithm1, Allocator1>& lhs,
    const nary_tree<T, Algorithm2, Allocator2>& rhs) {
    return rhs.operator==(lhs);
}

template <
    typename T,
    typename Algorithm1,
    typename Algorithm2,
    typename Allocator1,
    typename Allocator2>
bool operator!=(
    const nary_tree<T, Algorithm2, Allocator2>& lhs,
    const binary_tree<T, Algorithm1, Allocator1>& rhs) {
    return !lhs.operator==(rhs);
}

template <
    typename T,
    typename Algorithm1,
    typename Algorithm2,
    typename Allocator1,
    typename Allocator2>
bool operator!=(
    const binary_tree<T, Algorithm1, Allocator1>& lhs,
    const nary_tree<T, Algorithm2, Allocator2>& rhs) {
    return !rhs.operator==(lhs);
}

} // namespace ds
