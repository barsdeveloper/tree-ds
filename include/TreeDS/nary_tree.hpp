#pragma once

#include <memory> // std::allocator

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
};

} // namespace ds
