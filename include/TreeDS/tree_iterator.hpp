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
    using navigator_type     = decltype(std::declval<tree_type>().get_node_navigator());
    using actual_policy_type = decltype(
        std::declval<Policy>().get_instance(
            std::declval<node_type*>(),
            std::declval<navigator_type>(),
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
    tree_iterator(Policy policy, tree_type& tree, node_type* current_node = nullptr) :
            policy(policy.get_instance(current_node, tree.get_node_navigator(), tree.get_allocator())),
            pointed_tree(&tree) {
    }

    template <bool OtherConstant>
    tree_iterator(
        const tree_iterator<Tree, Policy, OtherConstant>& other,
        tree_type* tree,
        node_type* current = nullptr) :
            policy(other.policy, current),
            pointed_tree(tree) {
    }

    template <bool C = Constant, typename = std::enable_if_t<C>>
    tree_iterator<Tree, Policy, !C> craft_non_constant_iterator() const {
        return tree_iterator<Tree, Policy, !C>(
            *this,
            const_cast<std::decay_t<Tree>*>(this->pointed_tree));
    }

    public:
    // Iterators must be default constructible
    constexpr tree_iterator() :
            policy(Policy().get_instance(
                static_cast<node_type*>(nullptr),
                navigator_type(),
                typename tree_type::allocator_type())) {
    }

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
        this->policy       = actual_policy_type(other.policy);
        this->pointed_tree = other.pointed_tree;
        return *this;
    }

    template <typename OtherPolicy>
    tree_iterator<Tree, OtherPolicy, Constant> other_policy(OtherPolicy policy) {
        return this->pointed_tree != nullptr
            ? tree_iterator<Tree, OtherPolicy, Constant>(policy, *this->pointed_tree, this->policy.get_current_node())
            : tree_iterator<Tree, OtherPolicy, Constant>();
    }

    const node_type* get_node() const {
        return this->policy.get_current_node();
    }

    node_type* get_node() {
        return this->policy.get_current_node();
    }

    value_type& operator*() {
        return this->policy.get_current_node()->value;
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
            this->policy.decrement();
        } else if (this->pointed_tree != nullptr && this->pointed_tree->get_root() != nullptr) {
            /*
             * If iterator is at the end():
             *     normal iterator  => decremented from end() => go to its last element (before end())
             *     reverse iterator => incremented from end() => go to its first element (rewind)
             * REMEMBER: ++ operator on a reverse_iterator delegates to -- operator of tree_iterator and vice versa
             */
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

namespace detail {
    template <
        typename Tree,
        typename Policy,
        bool Constant = false>
    class tree_iterator_node_return : public tree_iterator<Tree, Policy, Constant> {

        using tree_iterator<Tree, Policy, Constant>::tree_iterator;
        using node_type = typename tree_iterator<Tree, Policy, Constant>::node_type;

        node_type& operator*() {
            return this->get_node()->value;
        }

        const node_type& operator*() const {
            return this->get_node();
        }

        node_type* operator->() {
            return &(this->get_node());
        }

        const node_type* operator->() const {
            return &(this->get_node());
        }
    };

} // namespace detail

} // namespace md
