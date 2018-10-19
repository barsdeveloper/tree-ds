#pragma once

#include <memory> // std::allocator

#include <TreeDS/node/binary_node.hpp>
#include <TreeDS/traversal_policy/pre_order.hpp>
#include <TreeDS/tree.hpp>

namespace ds {

/**
 * @brief A type of tree having nodes {@link binary_node}.
 *
 * @tparam T the type of value hold by this tree
 * @tparam Algorithm default traversal algorithm
 * @tparam Allocator the allocator used to allocate nodes
 * @relates tree
 */
template <
    typename T,
    typename Algorithm = pre_order,
    typename Allocator = std::allocator<T>>
class binary_tree : public tree<T, binary_node<T>, Algorithm, Allocator> {
    using super = tree<T, binary_node<T>, Algorithm, Allocator>;

    public:
    using tree<T, binary_node<T>, Algorithm, Allocator>::tree;
    using tree<T, binary_node<T>, Algorithm, Allocator>::operator=;
};

} // namespace ds
