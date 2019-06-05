#pragma once

#include <TreeDS/tree_base.hpp>
#include <TreeDS/viewer/tree_view.hpp>

namespace md {

template <
    typename T,
    typename Policy    = default_policy,
    typename Allocator = std::allocator<T>>
class binary_tree_view : public tree_view<binary_node<T>, Policy, Allocator> {

    using tree_view<binary_node<T>, Policy, Allocator>::tree_view;

    public:
    DECLARE_TREEDS_TYPES(const binary_node<T>, Policy, Allocator)
};

} // namespace md
