#pragma once

#include <functional> // mem_fn()
#include <optional>
#include <type_traits> // std::conditional_t, std::is_constructible_v, std::enable_if_t, std::void_t

#include <TreeDS/node/binary_node.hpp>

namespace md {

// TODO C++20 Remove this class and use a range
template <typename Predicate, typename TreeIterator>
class tree_iterator_filter {

    /*   ---   TYPES   ---   */
    public:
    using actual_policy_type = typename TreeIterator::actual_policy_type;
    using navigator_type     = typename TreeIterator::navigator_type;
    using node_type          = typename TreeIterator::node_type;
    using policy_type        = typename TreeIterator::policy_type;
    using tree_type          = typename TreeIterator::tree_type;
    // Iterators mandatory type declarations
    using difference_type   = typename TreeIterator::difference_type;
    using iterator_category = typename TreeIterator::iterator_category;
    using pointer           = typename TreeIterator::pointer;
    using reference         = typename TreeIterator::reference;
    using value_type        = typename TreeIterator::value_type;

    /*   ---   ATTRIBUTES   ---   */
    protected:
    std::optional<Predicate> predicate;
    TreeIterator iterator {};

    /*   ---   CONSTRUCTORS   ---   */
    public:
    /// @brief Constructs a tree_iterator_filter pointing nowhere and not associated to any tree
    constexpr tree_iterator_filter() {
    }

    tree_iterator_filter(const Predicate& predicate, const TreeIterator& it) :
            predicate(predicate),
            iterator(it) {
        while (this->iterator && !(*this->predicate)(*this->iterator)) {
            ++this->iterator;
        }
    }

    tree_iterator_filter(tree_iterator_filter& other) :
            predicate(other.predicate),
            iterator(other.iterator) {
    }

    template <
        typename OtherTreeIterator,
        typename = std::enable_if_t<std::is_constructible_v<TreeIterator, const OtherTreeIterator&>>>
    tree_iterator_filter(const tree_iterator_filter<Predicate, OtherTreeIterator>& other) :
            predicate(other.predicate),
            iterator(other.iterator) {
    }

    template <
        typename OtherTreeIterator,
        typename = std::enable_if_t<std::is_constructible_v<TreeIterator, OtherTreeIterator&&>>>
    tree_iterator_filter(tree_iterator_filter<Predicate, OtherTreeIterator>&& other) :
            predicate(other.predicate),
            iterator(std::move(other.iterator)) {
    }

    tree_iterator_filter(tree_iterator_filter&& other) :
            predicate(other.predicate),
            iterator(std::move(other.iterator)) {
    }

    tree_iterator_filter& operator=(const tree_iterator_filter& other) {
        this->iterator = other.iterator;
        return *this;
    }

    template <
        typename OtherTreeIterator,
        typename = std::enable_if_t<std::is_assignable_v<TreeIterator, const OtherTreeIterator&>>>
    tree_iterator_filter& operator=(const tree_iterator_filter<Predicate, OtherTreeIterator>& other) {
        this->iterator = other.iterator;
        return *this;
    }

    tree_iterator_filter& operator=(tree_iterator_filter&& other) {
        this->iterator = std::move(other.iterator);
        return *this;
    }

    template <
        typename OtherTreeIterator,
        typename = std::enable_if_t<std::is_assignable_v<TreeIterator, OtherTreeIterator&&>>>
    tree_iterator_filter& operator=(tree_iterator_filter<Predicate, OtherTreeIterator>&& other) {
        this->iterator = std::move(other.iterator);
        return *this;
    }

    tree_iterator_filter other_node(node_type* node) const {
        return {*this->predicate, iterator.other_node(node)};
    }

    template <typename OtherPolicy>
    tree_iterator_filter<Predicate, decltype(iterator.other_policy(std::declval<OtherPolicy>()))>
    other_policy(OtherPolicy p) const {
        return {*this->predicate, iterator.other_policy(p)};
    }

    template <typename OtherNavigator>
    tree_iterator_filter<Predicate, decltype(iterator.other_navigator(std::declval<OtherNavigator>()))>
    other_navigator(OtherNavigator n) const {
        return {*this->predicate, iterator.other_navigator(n)};
    }

    /*   ---   METHODS   ---   */
    public:
    const TreeIterator& base() const {
        return this->iterator;
    }

    TreeIterator& base() {
        return this->iterator;
    }

    navigator_type get_navigator() const {
        return this->iterator.get_navigator();
    }

    const node_type* get_raw_root() const {
        return this->iterator.get_raw_root();
    }

    const node_type* get_raw_node() const {
        return this->iterator.get_raw_node();
    }

    node_type* get_raw_node() {
        return this->iterator.get_raw_node();
    }

    decltype(std::declval<TreeIterator>().get_policy())
    get_policy() const {
        return this->iterator.get_policy();
    }

    const actual_policy_type& get_raw_policy() const {
        return this->iterator.get_raw_policy();
    }

    actual_policy_type& get_raw_policy() {
        return this->iterator.get_raw_policy();
    }

    value_type& operator*() {
        return this->iterator.operator*();
    }

    const value_type& operator*() const {
        return this->iterator.operator*();
    }

    value_type* operator->() {
        return this->iterator.operator->();
    }

    const value_type* operator->() const {
        return this->iterator.operator->();
    }

    /*   ---   COMPARISON   ---   */
    template <
        typename OtherIterator,
        typename = std::void_t<decltype(std::declval<TreeIterator>() == std::declval<const OtherIterator&>().base())>>
    bool operator==(const tree_iterator_filter<Predicate, OtherIterator>& other) const {
        return this->iterator.operator==(other.base());
    }

    template <
        typename OtherIterator,
        typename = std::void_t<decltype(std::declval<TreeIterator>() != std::declval<const OtherIterator&>().base())>>
    bool operator!=(const tree_iterator_filter<Predicate, OtherIterator>& other) const {
        return this->iterator.operator!=(other.base());
    }

    tree_iterator_filter& go_parent() {
        return this->iterator.go_parent();
    }

    tree_iterator_filter& go_prev_sibling() {
        return this->iterator.go_prev_sibling();
    }

    tree_iterator_filter& go_next_sibling() {
        this->iterator.go_next_sibling();
        return *this;
    }

    tree_iterator_filter& go_first_child() {
        this->iterator.go_first_child();
        return *this;
    }

    tree_iterator_filter& go_last_child() {
        this->iterator.go_last_child();
        return *this;
    }

    tree_iterator_filter& go_child(std::size_t index) {
        this->iterator.go_child(index);
        return *this;
    }

    tree_iterator_filter& operator++() {
        do {
            this->iterator.operator++();
        } while (*this && !(*this->predicate)(*this->iterator));
        return *this;
    }

    tree_iterator_filter operator++(int) {
        do {
            this->iterator.operator++(0);
        } while (*this && !(*this->predicate)(*this->iterator));
        return *this;
    }

    tree_iterator_filter& operator--() {
        do {
            this->iterator.TreeIterator::operator--();
        } while (*this && !(*this->predicate)(*this->iterator));
        return *this;
    }

    tree_iterator_filter operator--(int) {
        do {
            this->iterator.TreeIterator::operator--(0);
        } while (*this && !(*this->predicate)(*this->iterator));
        return *this;
    }

    void update(node_type& current, node_type* replacement) {
        this->iterator.update(current, replacement);
        return *this;
    }

    operator bool() const {
        return this->iterator.operator bool();
    }
};

} // namespace md
