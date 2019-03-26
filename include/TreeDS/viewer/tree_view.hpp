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

    using super = basic_tree<T, const Node, Policy, Allocator>;

    public:
    DECLARE_TYPES(T, const Node, Policy, Allocator)

    tree_view() :
            super(nullptr, 0, 0) {
    }

    template <typename TreePolicy>
    tree_view(const tree<T, Node, TreePolicy, Allocator>& tree) :
            super(tree.get_root(), tree.size(), tree.arity()) {
    }

    template <typename ViewPolicy>
    tree_view(const tree_view<T, Node, ViewPolicy, Allocator>& tree) :
            super(tree.get_root(), tree.size(), tree.arity()) {
    }

    template <
        typename TreePolicy,
        typename IteratorPolicy,
        bool Constant>
    tree_view(
        const tree<T, Node, TreePolicy, Allocator>& tree,
        const tree_iterator<
            basic_tree<T, Node, TreePolicy, Allocator>, // tree_iterator always uses basic_tree, no subclasses
            IteratorPolicy,
            Constant>& position) :
            super(
                position.get_node(),
                position.get_node()
                    ? position.get_node()->is_root()
                        ? tree.size()
                        : 0u
                    : 0u,
                position.get_node()
                    ? position.get_node()->is_root()
                        ? tree.arity()
                        : 0u
                    : 0u) {
        if (!tree.is_own_iterator(position)) {
            throw std::logic_error("Tried to create an nary_tree_biew with an iterator not belonging to the tree.");
        }
        if (position.get_node() && position.get_node()->is_root_limit(*tree.get_root())) {
            this->size_value  = tree.size();
            this->arity_value = tree.arity();
        }
    }

    template <
        typename ViewPolicy,
        typename IteratorPolicy,
        bool Constant>
    tree_view(
        const tree_view<T, Node, ViewPolicy, Allocator>& tree,
        const tree_iterator<
            basic_tree<T, const Node, ViewPolicy, Allocator>, // tree_iterator always uses basic_tree, no subclasses
            IteratorPolicy,
            Constant>& position) :
            super(
                position.get_node(), 0u, 0u) {
        if (!tree.is_own_iterator(position)) {
            throw std::logic_error("Tried to create an nary_tree_biew with an iterator not belonging to the tree.");
        }
        if (position.get_node() && position.get_node()->is_root_limit(*tree.get_root())) {
            this->size_value  = tree.size();
            this->arity_value = tree.arity();
        }
    }
};

} // namespace md
