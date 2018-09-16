#pragma once

#include <memory> // std::allocator

namespace ds {

// Forward declarations
template <typename, template <typename> class, typename, typename>
class tree;

template <typename>
class binary_node;

class pre_order;

/**
 * @brief A type of tree having nodes {@link binary_node}.
 * 
 * @tparam T the type of value hold by this tree
 * @tparam pre_order default traversal algorithm
 * @tparam std::allocator<T> the allocator used to allocate nodes
 * @relates tree
 */
template <
    typename T,
    typename Algorithm = pre_order,
    typename Allocator = std::allocator<T>>
class binary_tree : public tree<T, binary_node, Algorithm, Allocator> {
    using super = tree<T, binary_node, Algorithm, Allocator>;
    using tree<T, binary_node, Algorithm, Allocator>::tree;
};

} // namespace ds