#pragma once

#include <memory>    // std::allocator
#include <stdexcept> // std::logic_error

#include <TreeDS/basic_tree.hpp>
#include <TreeDS/policy/pre_order.hpp>
#include <TreeDS/tree_iterator.hpp>
#include <TreeDS/utility.hpp>

namespace md {

template <typename T, typename Policy = pre_order, typename Allocator = std::allocator<T>>
class nary_tree_view : public basic_tree<T, const nary_node<T>, Policy, Allocator> {

    using super = basic_tree<T, const nary_node<T>, Policy, Allocator>;

    public:
    DECLARE_TYPES(T, const nary_node<T>, Policy, Allocator)

    template <typename Node, typename OtherPolicy, CHECK_NARY_NODE(Node)>
    nary_tree_view(const basic_tree<T, Node, OtherPolicy, Allocator>& tree) :
            super(tree.get_root(), tree.size(), tree.arity()) {
    }

    template <
        typename Node,
        typename TreePolicy,
        typename IteratorPolicy,
        bool Constant,
        CHECK_NARY_NODE(Node)>
    nary_tree_view(
        const basic_tree<T, Node, TreePolicy, Allocator>& tree,
        const tree_iterator<basic_tree<T, Node, TreePolicy, Allocator>, IteratorPolicy, Constant>& position) :
            super(position.get_node(), 0u, 0u) {
        if (!tree.is_own_iterator(position)) {
            throw std::logic_error("Tried to create an nary_tree_biew with an iterator not belonging to the tree.");
        }
        if (position.get_node()) {
            const nary_node<T>& node = *position.get_node();
            this->size_value         = calculate_size(node);
            this->arity_value        = calculate_arity(node, tree.arity());
        }
    }
};

} // namespace md
