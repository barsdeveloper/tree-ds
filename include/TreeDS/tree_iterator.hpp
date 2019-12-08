#pragma once

#include <functional>  // mem_fn()
#include <type_traits> // std::conditional_t, std::enable_if_t

#include <TreeDS/node/binary_node.hpp>

namespace md {

template <typename, typename, typename>
class tree_base;

template <typename, typename, typename>
class tree;

template <typename Tree, typename Policy, typename NodeNavigator>
class tree_iterator {

    /*   ---   FRIENDS   ---   */
    template <typename, typename, typename>
    friend class tree_base;

    template <typename, typename, typename>
    friend class tree;

    template <typename, typename, typename>
    friend class tree_iterator;

    /*   ---   TYPES   ---   */
    public:
    static constexpr bool IS_CONSTANT_ITERATOR = std::is_const_v<Tree>;
    using tree_type                            = Tree;
    using node_type                            = std::conditional_t<
        IS_CONSTANT_ITERATOR,
        const typename tree_type::node_type,
        typename tree_type::node_type>;
    using navigator_type     = NodeNavigator;
    using policy_type        = Policy;
    using actual_policy_type = decltype(
        std::declval<Policy>().get_instance(
            std::declval<node_type*>(),
            std::declval<navigator_type>(),
            std::declval<typename Tree::node_allocator_type>()));
    // Iterators mandatory type declarations
    using value_type = std::conditional_t<
        IS_CONSTANT_ITERATOR,
        const typename tree_type::value_type,
        typename tree_type::value_type>;
    using difference_type   = std::ptrdiff_t;
    using pointer           = value_type*;
    using reference         = value_type&;
    using iterator_category = std::bidirectional_iterator_tag;

    /*   ---   VALIDATION   ---   */
    static_assert(
        std::is_same_v<typename NodeNavigator::node_type, node_type>,
        "Ill-formed type: the navigator must have the same node type as the tree");

    /*   ---   ATTRIBUTES   ---   */
    protected:
    actual_policy_type policy {};

    /*   ---   CONSTRUCTORS   ---   */
    public:
    /// @brief Constructs a tree_iterator pointing nowhere and not associated to any tree
    constexpr tree_iterator() {
    }

    tree_iterator(const actual_policy_type& policy) :
            policy(policy) {
    }

    tree_iterator(tree_type& tree) :
            policy(Policy().get_instance(
                static_cast<node_type*>(nullptr),
                navigator_type(tree.raw_root_node()),
                tree.get_node_allocator())) {
    }

    /**
     * @brief Constructors a and iterator associated to the given tree and possibly pointing somewhere
     * @param tree the tree associated
     * @param current_node the navigator pointing to the given position
     */
    template <
        typename OtherNodeNavigator = navigator_type,
        typename                    = std::enable_if_t<std::is_convertible_v<OtherNodeNavigator, navigator_type>>>
    tree_iterator(tree_type& tree, node_type* current_node, OtherNodeNavigator navigator) :
            policy(Policy().get_instance(current_node, navigator, tree.get_node_allocator())) {
    }

    // Conversion constructor from iterator to const_iterator
    template <
        typename OtherTree,
        typename OtherNodeNavigator,
        typename = std::enable_if_t<std::is_convertible_v<OtherTree*, Tree*>>,
        typename = std::enable_if_t<std::is_convertible_v<OtherNodeNavigator, NodeNavigator>>>
    tree_iterator(const tree_iterator<OtherTree, Policy, OtherNodeNavigator>& other) :
            policy(other.policy) {
    }

    // Conversion constructor from iterator to const_iterator
    template <
        typename OtherTree,
        typename OtherNodeNavigator,
        typename = std::enable_if_t<std::is_convertible_v<OtherTree*, Tree*>>,
        typename = std::enable_if_t<std::is_convertible_v<OtherNodeNavigator, NodeNavigator>>>
    tree_iterator(
        const tree_iterator<OtherTree, Policy, OtherNodeNavigator>& other,
        node_type* current_node) :
            policy(other.policy, current_node) {
    }

    /*   ---   ASSIGNMENT   ---   */
    // Conversion copy assignment from iterator to const_iterator
    template <
        typename OtherTree,
        typename OtherNodeNavigator,
        typename = std::enable_if_t<std::is_convertible_v<OtherTree*, Tree*>>,
        typename = std::enable_if_t<std::is_convertible_v<OtherNodeNavigator, NodeNavigator>>>
    tree_iterator& operator=(const tree_iterator<OtherTree, Policy, OtherNodeNavigator>& other) {
        this->policy = other.policy;
        return *this;
    }

    /*   ---   FACTORY   ---   */
    tree_iterator other_node(node_type* node) const {
        return tree_iterator(*this, node);
    }

    template <typename OtherPolicy>
    tree_iterator<Tree, OtherPolicy, NodeNavigator>
    other_policy(OtherPolicy) const {
        return tree_iterator<Tree, OtherPolicy, NodeNavigator>(
            OtherPolicy().get_instance(this->get_raw_node(), this->get_navigator(), this->policy.get_allocator()));
    }

    template <typename OtherNavigator>
    tree_iterator<Tree, Policy, OtherNavigator>
    other_navigator(OtherNavigator navigator) const {
        return tree_iterator<Tree, Policy, OtherNavigator>(
            Policy().get_instance(this->get_raw_node(), navigator, this->policy.get_allocator()));
    }

    /*   ---   METHODS   ---   */
    protected:
    template <typename TargetType>
    TargetType craft_non_constant_iterator(type_value<TargetType>) const {
        static_assert(
            std::is_same_v<std::decay_t<node_type>, typename TargetType::node_type>,
            "The requsted type must have the same node type as this iterator");
        static_assert(
            std::is_same_v<std::decay_t<policy_type>, typename TargetType::policy_type>,
            "The requsted type must have the same node type as this iterator");
        return TargetType(
            typename TargetType::policy_type().get_instance(
                const_cast<typename TargetType::node_type*>(this->get_raw_node()),
                typename TargetType::navigator_type(const_cast<std::remove_const_t<node_type>*>(this->policy.get_navigator().get_root())),
                static_cast<typename TargetType::actual_policy_type::allocator_type>(this->policy.get_allocator())));
    }

    template <typename F, typename... AdditionalArgs>
    tree_iterator& do_move(F&& function, AdditionalArgs&&... args) {
        this->policy = actual_policy_type(
            this->policy,
            function(
                this->get_navigator(),
                this->get_raw_node(),
                std::forward<AdditionalArgs>(args)...));
        return *this;
    }

    public:
    navigator_type get_navigator() const {
        return this->policy.get_navigator();
    }

    node_type* get_raw_root() const {
        return this->get_navigator().get_root();
    }

    node_type* get_raw_node() const {
        return this->policy.get_current_node();
    }

    Policy get_policy() const {
        return Policy();
    }

    const actual_policy_type& get_raw_policy() const {
        return this->policy;
    }

    actual_policy_type& get_raw_policy() {
        return this->policy;
    }

    value_type& operator*() {
        return this->policy.get_current_node()->get_value();
    }

    const value_type& operator*() const {
        return this->get_raw_node()->get_value();
    }

    value_type* operator->() {
        return &(this->get_raw_node()->get_value());
    }

    const value_type* operator->() const {
        return &(this->get_raw_node()->get_value());
    }

    /*   ---   COMPARISON   ---   */
    template <
        typename OtherTree,
        typename OtherNodeNavigator,
        typename = std::enable_if_t<std::is_convertible_v<OtherTree*, Tree*> || std::is_convertible_v<Tree*, OtherTree*>>,
        typename = std::enable_if_t<std::is_convertible_v<OtherNodeNavigator, NodeNavigator> || std::is_convertible_v<NodeNavigator, OtherNodeNavigator>>>
    bool operator==(const tree_iterator<OtherTree, Policy, OtherNodeNavigator>& other) const {
        return this->get_raw_node() == other.get_raw_node() && this->get_raw_root() == other.get_raw_root();
    }

    template <
        typename OtherTree,
        typename OtherNodeNavigator,
        typename = std::enable_if_t<std::is_convertible_v<OtherTree*, Tree*> || std::is_convertible_v<Tree*, OtherTree*>>,
        typename = std::enable_if_t<std::is_convertible_v<OtherNodeNavigator, NodeNavigator> || std::is_convertible_v<NodeNavigator, OtherNodeNavigator>>>
    bool operator!=(const tree_iterator<OtherTree, Policy, OtherNodeNavigator>& other) const {
        return !this->operator==(other);
    }

    tree_iterator& go_parent() {
        return this->do_move(std::mem_fn(&navigator_type::get_parent));
    }

    tree_iterator& go_prev_sibling() {
        return this->do_move(std::mem_fn(&navigator_type::get_prev_sibling));
    }

    tree_iterator& go_next_sibling() {
        return this->do_move(std::mem_fn(&navigator_type::get_next_sibling));
    }

    tree_iterator& go_first_child() {
        return this->do_move(std::mem_fn(&navigator_type::get_first_child));
    }

    tree_iterator& go_last_child() {
        return this->do_move(std::mem_fn(&navigator_type::get_last_child));
    }

    tree_iterator& go_child(std::size_t index) {
        return this->do_move(std::mem_fn(&navigator_type::get_child), index);
    }

    tree_iterator& operator++() {
        if (this->get_raw_node()) {
            this->policy.increment();
        } else if (this->get_raw_root()) {
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
        if (this->policy.get_current_node()) {
            this->policy.decrement();
        } else if (this->get_raw_root()) {
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
        this->policy.update(&current, replacement);
    }

    operator bool() const {
        return static_cast<bool>(this->get_raw_node());
    }
};

// Conversion constructor from iterator to const_iterator
template <typename Tree, typename Policy, typename NodeNavigator>
tree_iterator(const tree_iterator<Tree, Policy, NodeNavigator>& other)
    ->tree_iterator<Tree, Policy, NodeNavigator>;

template <typename Tree, typename Policy, typename NodeNavigator>
tree_iterator(
    const tree_iterator<Tree, Policy, NodeNavigator>& other,
    typename tree_iterator<Tree, Policy, NodeNavigator>::node_type* current_node)
    ->tree_iterator<Tree, Policy, NodeNavigator>;

} // namespace md
