#pragma once

#include <stdexcept>   // std::logic_error
#include <type_traits> // std::is_same_v

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

    template <typename TreeNode, typename TreePolicy>
    tree_view(const basic_tree<T, TreeNode, TreePolicy, Allocator>& tree) :
            super(tree.root, tree.size_value, tree.arity_value, tree.navigator) {
        static_assert(
            std::is_convertible_v<std::add_pointer_t<TreeNode>, std::add_pointer_t<Node>>,
            "The view must refer to the same type of tree/view: either nary or binary.");
    }

    template <
        typename TreeNode,
        typename TreePolicy,
        typename IteratorTree,
        typename IteratorPolicy,
        bool IteratorConstant>
    tree_view(
        const basic_tree<T, TreeNode, TreePolicy, Allocator>& tree,
        const tree_iterator<IteratorTree, IteratorPolicy, IteratorConstant>& position) :
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
        static_assert(
            std::is_convertible_v<std::add_pointer_t<TreeNode>, std::add_pointer_t<Node>>,
            "The view must refer to the same type of tree/view: either nary or binary.");
        static_assert(
            std::is_same_v<std::decay_t<Node>, std::decay_t<typename IteratorTree::node_type>>,
            "The iterator must belong to the tree.");
        if (!tree.is_own_iterator(position)) {
            throw std::logic_error("Tried to create an nary_tree_view with an iterator not belonging to the tree.");
        }
    }
};

} // namespace md
