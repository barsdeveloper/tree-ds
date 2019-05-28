#pragma once

#include <memory> // std::allocator

#include <TreeDS/node/binary_node.hpp>
#include <TreeDS/policy/pre_order.hpp>
#include <TreeDS/tree.hpp>

namespace md {

/**
 * @brief A type of tree having nodes {@link binary_node}.
 *
 * @tparam T the type of value hold by this tree
 * @tparam Policy default traversal algorithm
 * @tparam Allocator the allocator used to allocate nodes
 * @relates tree
 */
template <
    typename T,
    typename Policy    = default_policy,
    typename Allocator = std::allocator<T>>
class binary_tree : public tree<binary_node<T>, Policy, Allocator> {
    using super = tree<binary_node<T>, Policy, Allocator>;

    public:
    using tree<binary_node<T>, Policy, Allocator>::tree;
    using tree<binary_node<T>, Policy, Allocator>::operator=;
};

} // namespace md
