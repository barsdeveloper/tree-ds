#pragma once

#include <memory> // std::allocator

#include <TreeDS/node/binary_node.hpp>
#include <TreeDS/policy/pre_order.hpp>
#include <TreeDS/tree_base.hpp>

namespace md::binary {

/**
 * @brief A type of tree having nodes {@link binary::node}.
 *
 * @tparam T the type of value hold by this tree
 * @tparam Policy default traversal algorithm
 * @tparam Allocator the allocator used to allocate nodes
 * @relates tree
 */
template <
    typename T,
    typename Policy    = pre_order,
    typename Allocator = std::allocator<T>>
class tree : public md::tree_base<T, binary::node<T>, Policy, Allocator> {
    using super = md::tree_base<T, binary::node<T>, Policy, Allocator>;

    public:
    using md::tree_base<T, binary::node<T>, Policy, Allocator>::tree_base;
    using md::tree_base<T, binary::node<T>, Policy, Allocator>::operator=;
};

} // namespace md::binary
