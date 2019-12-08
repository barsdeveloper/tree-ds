#pragma once

#include <functional> // mem_fn()
#include <optional>
#include <type_traits> // std::conditional_t, std::is_constructible_v, std::enable_if_t, std::void_t

#include <TreeDS/tree_iterator.hpp>

namespace md {

// TODO C++20 Remove this class and use a range
template <typename Predicate, typename Tree, typename Policy, typename NodeNavigator>
class tree_iterator_filter : public tree_iterator<Tree, Policy, NodeNavigator> {

    /*   ---   TYPES   ---   */
    public:
    using typename tree_iterator<Tree, Policy, NodeNavigator>::actual_policy_type;
    using typename tree_iterator<Tree, Policy, NodeNavigator>::navigator_type;
    using typename tree_iterator<Tree, Policy, NodeNavigator>::node_type;
    using typename tree_iterator<Tree, Policy, NodeNavigator>::policy_type;
    using typename tree_iterator<Tree, Policy, NodeNavigator>::tree_type;
    // Iterators mandatory type declarations
    using typename tree_iterator<Tree, Policy, NodeNavigator>::difference_type;
    using typename tree_iterator<Tree, Policy, NodeNavigator>::iterator_category;
    using typename tree_iterator<Tree, Policy, NodeNavigator>::pointer;
    using typename tree_iterator<Tree, Policy, NodeNavigator>::reference;
    using typename tree_iterator<Tree, Policy, NodeNavigator>::value_type;

    /*   ---   ATTRIBUTES   ---   */
    protected:
    std::optional<Predicate> predicate;

    /*   ---   CONSTRUCTORS   ---   */
    public:
    /// @brief Constructs a tree_iterator_filter pointing nowhere and not associated to any tree
    constexpr tree_iterator_filter() {
    }

    tree_iterator_filter(const Predicate& predicate, const tree_iterator<Tree, Policy, NodeNavigator>& it) :
            tree_iterator<Tree, Policy, NodeNavigator>(it),
            predicate(predicate) {
        if (*this && !(*this->predicate)(this->operator*())) {
            ++(*this);
        }
    }

    tree_iterator_filter(tree_iterator_filter& other) :
            tree_iterator<Tree, Policy, NodeNavigator>(other),
            predicate(other.predicate) {
    }

    /*template <
        typename OtherTreeIterator,
        typename = std::enable_if_t<std::is_constructible_v<TreeIterator, const OtherTreeIterator&>>>
    tree_iterator_filter(const tree_iterator_filter<Predicate, OtherTreeIterator>& other) :
            tree_iterator<Tree, Predicate, NodeNavigator>(other),
            predicate(other.predicate) {
    }

    template <
        typename OtherTreeIterator,
        typename = std::enable_if_t<std::is_constructible_v<TreeIterator, OtherTreeIterator&&>>>
    tree_iterator_filter(tree_iterator_filter<Predicate, OtherTreeIterator>&& other) :
            predicate(other.predicate),
            iterator(std::move(other.iterator)) {
    }*/

    tree_iterator_filter(tree_iterator_filter&& other) :
            tree_iterator<Tree, Policy, NodeNavigator>(std::move(other)),
            predicate(other.predicate) {
    }

    tree_iterator_filter& operator=(const tree_iterator_filter& other) {
        this->tree_iterator<Tree, Policy, NodeNavigator>::operator=(other);
        return *this;
    }

    /*template <
        typename OtherTreeIterator,
        typename = std::enable_if_t<std::is_assignable_v<TreeIterator, const OtherTreeIterator&>>>
    tree_iterator_filter& operator=(const tree_iterator_filter<Predicate, OtherTreeIterator>& other) {
        this->iterator = other.iterator;
        return *this;
    }*/

    tree_iterator_filter& operator=(tree_iterator_filter&& other) {
        this->tree_iterator<Tree, Policy, NodeNavigator>::operator=(std::move(other));
        return *this;
    }

    /*template <
        typename OtherTreeIterator,
        typename = std::enable_if_t<std::is_assignable_v<TreeIterator, OtherTreeIterator&&>>>
    tree_iterator_filter& operator=(tree_iterator_filter<Predicate, OtherTreeIterator>&& other) {
        this->iterator = std::move(other.iterator);
        return *this;
    }*/

    tree_iterator_filter other_node(node_type* node) const {
        return {*this->predicate, this->tree_iterator<Tree, Policy, NodeNavigator>::other_node(node)};
    }

    template <typename OtherPolicy>
    tree_iterator_filter<Predicate, Tree, OtherPolicy, NodeNavigator>
    other_policy(OtherPolicy p) const {
        return {*this->predicate, this->tree_iterator<Tree, Policy, NodeNavigator>::other_policy(p)};
    }

    template <typename OtherNavigator>
    tree_iterator_filter<Predicate, Tree, Policy, OtherNavigator>
    other_navigator(OtherNavigator n) const {
        return {*this->predicate, this->tree_iterator<Tree, Policy, NodeNavigator>::other_navigator(n)};
    }

    /*   ---   COMPARISON   ---   */
    public:
    tree_iterator_filter& operator++() {
        do {
            this->tree_iterator<Tree, Policy, NodeNavigator>::operator++();
        } while (*this && !(*this->predicate)(this->operator*()));
        return *this;
    }

    tree_iterator_filter operator++(int) {
        tree_iterator_filter copy(*this);
        this->operator++();
        return copy;
    }

    tree_iterator_filter& operator--() {
        do {
            this->tree_iterator<Tree, Policy, NodeNavigator>::operator--();
        } while (*this && !(*this->predicate)(this->operator*()));
        return *this;
    }

    tree_iterator_filter operator--(int) {
        tree_iterator_filter copy(*this);
        this->operator--();
        return copy;
    }
};

} // namespace md
