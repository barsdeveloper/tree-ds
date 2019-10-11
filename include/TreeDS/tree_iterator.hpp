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
    tree_type* pointed_tree = nullptr; // nullptr => no container associated (default iterator)

    /*   ---   CONSTRUCTORS   ---   */
    public:
    /// @brief Constructs a tree_iterator pointing nowhere and not associated to any tree
    constexpr tree_iterator() {
    }

    tree_iterator(tree_type& tree) :
            policy(Policy().get_instance(
                static_cast<node_type*>(nullptr),
                navigator_type(tree.raw_root_node()),
                tree.get_node_allocator())),
            pointed_tree(&tree) {
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
            policy(Policy().get_instance(current_node, navigator, tree.get_node_allocator())),
            pointed_tree(&tree) {
    }

    // Conversion constructor from iterator to const_iterator
    template <
        typename OtherTree,
        typename OtherNodeNavigator,
        typename = std::enable_if_t<std::is_convertible_v<OtherTree*, Tree*>>,
        typename = std::enable_if_t<std::is_convertible_v<OtherNodeNavigator, NodeNavigator>>>
    tree_iterator(const tree_iterator<OtherTree, Policy, OtherNodeNavigator>& other) :
            policy(other.policy),
            pointed_tree(other.pointed_tree) {
    }

    // Conversion copy assignment from iterator to const_iterator
    template <
        typename OtherTree,
        typename OtherNodeNavigator,
        typename = std::enable_if_t<std::is_convertible_v<OtherTree*, Tree*>>,
        typename = std::enable_if_t<std::is_convertible_v<OtherNodeNavigator, NodeNavigator>>>
    tree_iterator& operator=(const tree_iterator<OtherTree, Policy, OtherNodeNavigator>& other) {
        this->policy       = other.policy;
        this->pointed_tree = other.pointed_tree;
        return *this;
    }

    template <typename OtherPolicy>
    tree_iterator<Tree, OtherPolicy, NodeNavigator> other_policy(OtherPolicy) {
        return this->pointed_tree != nullptr
            ? tree_iterator<Tree, OtherPolicy, NodeNavigator>(*this->pointed_tree, this->get_raw_node(), this->get_navigator())
            : tree_iterator<Tree, OtherPolicy, NodeNavigator>();
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
            const_cast<std::remove_const_t<Tree>&>(*this->pointed_tree),
            const_cast<std::remove_const_t<node_type>*>(this->get_raw_node()),
            typename TargetType::navigator_type(const_cast<std::remove_const_t<node_type>*>(this->policy.get_navigator().get_root())));
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

    const node_type* get_raw_node() const {
        return this->policy.get_current_node();
    }

    node_type* get_raw_node() {
        return this->policy.get_current_node();
    }

    const actual_policy_type& get_raw_policy() const {
        return this->policy;
    }

    actual_policy_type& get_raw_policy() {
        return this->policy;
    }

    value_type& operator*() {
        return this->policy.get_current_node()->value;
    }

    const value_type& operator*() const {
        return this->get_raw_node()->value;
    }

    value_type* operator->() {
        return &(this->get_raw_node()->value);
    }

    const value_type* operator->() const {
        return &(this->get_raw_node()->value);
    }

    /*   ---   COMPARISON   ---   */
    template <
        typename OtherTree,
        typename OtherNodeNavigator,
        typename = std::enable_if_t<std::is_convertible_v<OtherTree*, Tree*> || std::is_convertible_v<Tree*, OtherTree*>>,
        typename = std::enable_if_t<std::is_convertible_v<OtherNodeNavigator, NodeNavigator> || std::is_convertible_v<NodeNavigator, OtherNodeNavigator>>>
    bool operator==(const tree_iterator<OtherTree, Policy, OtherNodeNavigator>& other) const {
        return this->pointed_tree == other.pointed_tree
            && this->get_raw_node() == other.get_raw_node();
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
        } else if (this->pointed_tree && this->pointed_tree->raw_root_node()) {
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
        } else if (this->pointed_tree && this->pointed_tree->raw_root_node()) {
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
};

} // namespace md
