#pragma once

#include <TreeDS/basic_tree.hpp>
#include <TreeDS/policy/pre_order.hpp>
#include <TreeDS/viewer/tree_view.hpp>

namespace md {

template <
    typename T,
    typename Policy    = default_policy,
    typename Allocator = std::allocator<T>>
class binary_tree_view : public tree_view<T, binary_node<T>, Policy, Allocator> {

    using tree_view<T, binary_node<T>, Policy, Allocator>::tree_view;

    public:
    DECLARE_TYPES(T, const binary_node<T>, Policy, Allocator)
};

} // namespace md
