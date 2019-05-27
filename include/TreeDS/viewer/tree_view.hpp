#pragma once

#include <stdexcept> // std::logic_error

#include <TreeDS/basic_tree.hpp>
#include <TreeDS/policy/pre_order.hpp>
#include <TreeDS/tree.hpp>
#include <TreeDS/tree_iterator.hpp>
#include <TreeDS/utility.hpp>

namespace md {

template <
    typename T,
    typename Node,
    typename Policy,
    typename Allocator>
class tree_view : public basic_tree<T, const Node, Policy, Allocator> {

    public:
    DECLARE_TREEDS_TYPES(T, const Node, Policy, Allocator)
    using super = basic_tree<T, const Node, Policy, Allocator>;

    tree_view() :
            super(nullptr, 0, 0, node_navigator<Node>(nullptr, false)) {
    }

    template <typename ViewPolicy>
    tree_view(const basic_tree<T, Node, ViewPolicy, Allocator>& tree) :
            super(tree.root, tree.size_value, tree.arity_value, tree.navigator) {
    }

    template <
        typename TreePolicy,
        typename IteratorPolicy,
        bool Constant>
    tree_view(
        const basic_tree<T, Node, TreePolicy, Allocator>& tree,
        const tree_iterator<
            basic_tree<T, Node, TreePolicy, Allocator>, // Tree_iterator always refers to basic_tree, no subclasses
            IteratorPolicy,
            Constant>& position) :
            super(
                position.get_raw_node(),
                position.get_raw_node()
                    ? tree.get_node_navigator().is_root(*position.get_raw_node())
                        ? tree.size_value
                        : 0u
                    : 0u,
                position.get_raw_node()
                    ? tree.get_node_navigator().is_root(*position.get_raw_node())
                        ? tree.arity_value
                        : 0u
                    : 0u,
                navigator_type(position.get_raw_node(), true)) {
        if (!tree.is_own_iterator(position)) {
            throw std::logic_error("Tried to create an nary_tree_biew with an iterator not belonging to the tree.");
        }
    }
};

} // namespace md
