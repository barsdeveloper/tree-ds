#pragma once

#include <iterator>    // std::iterator
#include <type_traits> // std::conditional, std::enable_if

#include <TreeDS/node/binary_node.hpp>

namespace md {

template <typename, typename, typename, typename>
class tree;

template <
    typename Tree,
    typename Policy = typename Tree::algorithm_type,
    bool Constant   = false>
class tree_iterator {

    template <typename, typename, typename, typename>
    friend class tree;

    template <typename, typename, bool>
    friend class tree_iterator;

    public:
    using tree_type = std::conditional_t<Constant, const Tree, Tree>;
    using node_type = std::conditional_t<
        Constant,
        const typename tree_type::node_type,
        typename tree_type::node_type>;
    using policy_type = Policy;
    // Iterators mandatory type declarations
    using value_type = std::conditional_t<
        Constant,
        const typename tree_type::value_type,
        typename tree_type::value_type>;
    using difference_type   = std::ptrdiff_t;
    using pointer           = value_type*;
    using reference         = value_type&;
    using iterator_category = std::bidirectional_iterator_tag;

    protected:
    policy_type policy {};
    tree_type* pointed_tree = nullptr; // nullptr => no container associated (default iterator)
    node_type* current_node = nullptr; // nullptr => end()

    protected:
    // Constructor used by tree to create an iterator
    tree_iterator(tree_type& tree) :
            pointed_tree(&tree) {
    }

    public:
    ~tree_iterator() = default;

    // Iterators must be default constructible
    constexpr tree_iterator() = default;

    // Iterators must be CopyConstructible
    tree_iterator(const tree_iterator&) = default;

    // Iterators must be CopyAssignable
    tree_iterator& operator=(const tree_iterator&) = default;

    // Conversion from iterator to const_iterator
    template <
        bool OtherConstant,
        typename = std::enable_if_t<Constant && !OtherConstant>>
    explicit tree_iterator(const tree_iterator<Tree, Policy, OtherConstant>& other) :
            pointed_tree(other.pointed_tree),
            current_node(other.current_node) {
    }

    template <
        bool OtherConstant,
        typename = std::enable_if_t<Constant && !OtherConstant>>
    tree_iterator& operator=(const tree_iterator<Tree, Policy, OtherConstant>& other) {
        this->pointed_tree = other.pointed_tree;
        this->current_node = other.current_node;
        return *this;
    }

    node_type* get_node() {
        return current_node;
    }

    const node_type* get_node() const {
        return current_node;
    }

    value_type& operator*() {
        return current_node->value;
    }

    const value_type& operator*() const {
        return current_node->value;
    }

    value_type* operator->() {
        return &(current_node->value);
    }

    const value_type* operator->() const {
        return &(current_node->value);
    }

    template <bool OtherConst>
    bool operator==(const tree_iterator<Tree, Policy, OtherConst>& other) const {
        return this->pointed_tree == other.pointed_tree
            && this->current_node == other.current_node;
    }

    template <bool OtherConst>
    bool operator!=(const tree_iterator<Tree, Policy, OtherConst>& other) const {
        return !this->operator==(other);
    }

    tree_iterator& operator++() {
        if (current_node) {
            // const_cast needed in case node_type is non const
            current_node = const_cast<node_type*>(policy.increment(*current_node));
        } else if (pointed_tree != nullptr && pointed_tree->get_root() != nullptr) {
            /*
             * If iterator is at the end():
             *     normal iterator  => incremented from end() => go to its first element (rewind)
             *     reverse iterator	=> decremented from end() => go to its last element (before end())
             * REMEMBER: ++ operator on a reverse_iterator delegates to -- operator of tree_iterator and vice versa
             */
            // const_cast needed in case node_type is non const
            current_node = const_cast<node_type*>(policy.go_first(*pointed_tree->get_root()));
        }
        return *this;
    }

    tree_iterator operator++(int) {
        tree_iterator it(*this);
        this->operator++();
        return it;
    }

    tree_iterator& operator--() {
        if (current_node) {
            // const_cast needed in case node_type is non constant
            current_node = const_cast<node_type*>(policy.decrement(*current_node));
        } else if (pointed_tree != nullptr && pointed_tree->get_root() != nullptr) {
            /*
             * If iterator is at the end():
             *     normal iterator  => decremented from end() => go to its last element (before end())
             *     reverse iterator => incremented from end() => go to its first element (rewind)
             * REMEMBER: ++ operator on a reverse_iterator delegates to -- operator of tree_iterator and vice versa
             */
            // const_cast needed in case node_type is non const
            current_node = const_cast<node_type*>(policy.go_last(*pointed_tree->get_root()));
        }
        return *this;
    }

    tree_iterator operator--(int) {
        tree_iterator it(*this);
        this->operator--();
        return it;
    }

    void update(const node_type& current, const node_type& replacement) {
        current_node = const_cast<node_type*>(&replacement);
        if constexpr (is_updateable<policy_type, node_type>::value) {
            policy.update(current, replacement);
        }
    }
};

} // namespace ds
