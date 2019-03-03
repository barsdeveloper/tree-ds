#pragma once

#include <iterator>    // std::iterator
#include <type_traits> // std::conditional, std::enable_if

#include <TreeDS/node/binary_node.hpp>

namespace md {

template <typename, typename, typename, typename>
class basic_tree;

template <typename, typename, typename, typename>
class tree;

template <
    typename Tree,
    typename Policy,
    bool Constant = false>
class tree_iterator {

    template <typename, typename, typename, typename>
    friend class basic_tree;

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
    using actual_policy_type = decltype(
        std::declval<Policy>().get_instance(
            std::declval<node_type*>(),
            std::declval<node_type*>(),
            std::declval<typename Tree::allocator_type>()));
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
    actual_policy_type policy;
    tree_type* pointed_tree = nullptr; // nullptr => no container associated (default iterator)

    protected:
    // Constructor used by tree to create an iterator
    tree_iterator(tree_type& tree) :
            policy(
                Policy().get_instance(
                    tree.get_root(), static_cast<node_type*>(nullptr), tree.get_allocator())),
            pointed_tree(&tree) {
    }

    tree_iterator(tree_type* tree, node_type* current) :
            policy(
                tree != nullptr
                    ? Policy().get_instance(tree->get_root(), current, tree->get_allocator())
                    : Policy().get_instance(
                          static_cast<node_type*>(nullptr),
                          static_cast<node_type*>(nullptr),
                          typename tree_type::allocator_type())),
            pointed_tree(tree) {
        assert((tree == nullptr) == (current == nullptr));
        assert(!tree || (tree->get_root() == nullptr) == (current == nullptr));
    }

    template <bool C = Constant, typename = std::enable_if_t<C>>
    tree_iterator<Tree, Policy, !C> craft_non_constant_iterator() const {
        tree_iterator<Tree, Policy, !C> result;
        result.policy       = this->policy;
        result.pointed_tree = const_cast<Tree*>(this->pointed_tree);
        return result;
    }

    public:
    // Iterators must be default constructible
    constexpr tree_iterator() = default;

    // Iterators must be CopyConstructible
    tree_iterator(const tree_iterator&) = default;

    ~tree_iterator() = default;

    // Iterators must be CopyAssignable
    tree_iterator& operator=(const tree_iterator&) = default;

    // Conversion constructor from iterator to const_iterator
    template <
        bool OtherConstant,
        typename = std::enable_if_t<Constant && !OtherConstant>>
    tree_iterator(const tree_iterator<Tree, Policy, OtherConstant>& other) :
            policy(other.policy),
            pointed_tree(other.pointed_tree) {
    }

    // Conversion copy assignment from iterator to const_iterator
    template <
        bool OtherConstant,
        typename = std::enable_if_t<Constant && !OtherConstant>>
    tree_iterator& operator=(const tree_iterator<Tree, Policy, OtherConstant>& other) {
        this->policy       = other.policy;
        this->pointed_tree = other.pointed_tree;
        return *this;
    }

    template <typename OtherPolicy>
    tree_iterator<Tree, OtherPolicy, Constant> other_policy(OtherPolicy) {
        return this->pointed_tree != nullptr
            ? tree_iterator<Tree, OtherPolicy, Constant>(this->pointed_tree, this->get_node())
            : tree_iterator<Tree, OtherPolicy, Constant>();
    }

    node_type* get_node() {
        return const_cast<node_type*>(this->policy.get_current_node());
    }

    const node_type* get_node() const {
        return this->policy.get_current_node();
    }

    value_type& operator*() {
        return this->get_node()->value;
    }

    const value_type& operator*() const {
        return this->get_node()->value;
    }

    value_type* operator->() {
        return &(this->get_node()->value);
    }

    const value_type* operator->() const {
        return &(this->get_node()->value);
    }

    template <bool OtherConst>
    bool operator==(const tree_iterator<Tree, Policy, OtherConst>& other) const {
        return this->pointed_tree == other.pointed_tree
            && this->policy.get_current_node() == other.policy.get_current_node();
    }

    template <bool OtherConst>
    bool operator!=(const tree_iterator<Tree, Policy, OtherConst>& other) const {
        return !this->operator==(other);
    }

    tree_iterator& operator++() {
        if (this->policy.get_current_node() != nullptr) {
            this->policy.increment();
        } else if (this->pointed_tree != nullptr && this->pointed_tree->get_root() != nullptr) {
            /*
             * If iterator is at the end():
             *     normal iterator  => incremented from end() => go to its first element (rewind)
             *     reverse iterator	=> decremented from end() => go to its last element (before end())
             * REMEMBER: ++ operator on a reverse_iterator delegates to -- operator of tree_iterator and vice versa
             */
            // const_cast needed in case node_type is non const
            this->policy.go_first();
        }
        return *this;
    }

    tree_iterator operator++(int) {
        tree_iterator it(*this);
        this->operator++();
        return it;
    }

    tree_iterator& operator--() {
        if (this->policy.get_current_node() != nullptr) {
            // const_cast needed in case node_type is non constant
            this->policy.decrement();
        } else if (this->pointed_tree != nullptr && this->pointed_tree->get_root() != nullptr) {
            /*
             * If iterator is at the end():
             *     normal iterator  => decremented from end() => go to its last element (before end())
             *     reverse iterator => incremented from end() => go to its first element (rewind)
             * REMEMBER: ++ operator on a reverse_iterator delegates to -- operator of tree_iterator and vice versa
             */
            // const_cast needed in case node_type is non const
            this->policy.go_last();
        }
        return *this;
    }

    tree_iterator operator--(int) {
        tree_iterator it(*this);
        this->operator--();
        return it;
    }

    void update(node_type& current, node_type* replacement) {
        this->policy.update(current, replacement);
    }
};

} // namespace md
