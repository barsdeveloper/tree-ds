#pragma once

#define DECLARE_TREEDS_TYPES(NODE_T, POLICY_T, ALLOCATOR_T)                                                   \
    using typename tree_base<NODE_T, POLICY_T, ALLOCATOR_T>::value_type;                                      \
    using typename tree_base<NODE_T, POLICY_T, ALLOCATOR_T>::reference;                                       \
    using typename tree_base<NODE_T, POLICY_T, ALLOCATOR_T>::const_reference;                                 \
    using typename tree_base<NODE_T, POLICY_T, ALLOCATOR_T>::node_type;                                       \
    using typename tree_base<NODE_T, POLICY_T, ALLOCATOR_T>::size_type;                                       \
    using typename tree_base<NODE_T, POLICY_T, ALLOCATOR_T>::difference_type;                                 \
    using typename tree_base<NODE_T, POLICY_T, ALLOCATOR_T>::pointer;                                         \
    using typename tree_base<NODE_T, POLICY_T, ALLOCATOR_T>::const_pointer;                                   \
    using typename tree_base<NODE_T, POLICY_T, ALLOCATOR_T>::policy_type;                                     \
    using typename tree_base<NODE_T, POLICY_T, ALLOCATOR_T>::navigator_type;                                  \
    using typename tree_base<NODE_T, POLICY_T, ALLOCATOR_T>::allocator_type;                                  \
    using typename tree_base<NODE_T, POLICY_T, ALLOCATOR_T>::node_allocator_type;                             \
    template <typename P>                                                                                     \
    using iterator = typename tree_base<NODE_T, POLICY_T, ALLOCATOR_T>::template iterator<P>;                 \
    template <typename P>                                                                                     \
    using const_iterator = typename tree_base<NODE_T, POLICY_T, ALLOCATOR_T>::template const_iterator<P>;     \
    template <typename P>                                                                                     \
    using reverse_iterator = typename tree_base<NODE_T, POLICY_T, ALLOCATOR_T>::template reverse_iterator<P>; \
    template <typename P>                                                                                     \
    using const_reverse_iterator = typename tree_base<NODE_T, POLICY_T, ALLOCATOR_T>::template const_reverse_iterator<P>;

#include <cstddef>
#include <iterator> // std::make_reverse_iterator
#include <limits>   // std::numeric_limits
#include <memory>   // std::unique_ptr, std::allocator_traits
#include <ostream>  // std::ostream

#include <TreeDS/node/node_navigator.hpp>
#include <TreeDS/policy/breadth_first.hpp>
#include <TreeDS/policy/fixed.hpp>
#include <TreeDS/tree_iterator.hpp>

namespace md {

using default_policy = policy::breadth_first;
template <typename Node>
using node_value_t = std::remove_reference_t<decltype(std::declval<Node>().get_value())>;

template <typename, typename, typename>
class tree;

template <typename Node, typename Policy, typename Allocator>
class tree_base {

    /*  ---   FRIENDS   ---   */
    template <typename, typename, typename>
    friend class tree_base;

    template <typename, typename, typename>
    friend class tree;

    template <typename, typename, typename>
    friend class tree_view;

    template <typename, typename, bool>
    friend class tree_iterator;

    template <typename>
    friend class pattern;

    /*   ---   TYPES   ---   */
    public:
    // General
    using value_type          = std::remove_reference_t<decltype(std::declval<Node>().get_value())>;
    using reference           = value_type&;
    using const_reference     = const value_type&;
    using node_type           = Node;
    using size_type           = std::size_t;
    using difference_type     = std::ptrdiff_t;
    using pointer             = value_type*;
    using const_pointer       = const value_type*;
    using policy_type         = Policy;
    using navigator_type      = node_navigator<node_type>;
    using allocator_type      = Allocator;
    using node_allocator_type = typename std::allocator_traits<Allocator>::template rebind_alloc<std::decay_t<node_type>>;

    static_assert(is_tag_of_policy<Policy>, "Invalid Policy template parameter, pick one from namespace md::policy");
    static_assert(
        std::is_same_v<std::decay_t<value_type>, allocator_value_type<Allocator>>,
        "Invalid allocator::value_type");

    // Iterators
    template <typename P>
    using iterator = tree_iterator<tree_base, P, false>;
    template <typename P>
    using const_iterator = tree_iterator<tree_base, P, true>;
    template <typename P>
    using reverse_iterator = std::reverse_iterator<iterator<P>>;
    template <typename P>
    using const_reverse_iterator = std::reverse_iterator<const_iterator<P>>;

    /*   ---   ATTRIBUTES   ---   */
    protected:
    /// @brief Owning pointer to the root node.
    node_type* root = nullptr;
    /// @brief The number of nodes in the tree.
    mutable size_type size_value = 0u;
    /// @brief Maximum number of children a node can have.
    mutable size_type arity_value = 0u;
    /// @brief An object used to navigate the nodes.
    navigator_type navigator {};
    /// @brief Allocator object used to allocate the nodes.
    node_allocator_type allocator {};
    /// @brief Allocator object used by the method get_allocator to return the allocator supplied.
    allocator_type useless_allocator {};

    /*   ---   CONSTRUCTORS   ---   */
    protected:
    tree_base() {
    }

    tree_base(node_type* root, size_type size, size_type arity, navigator_type navigator) :
            root(root),
            size_value(size),
            arity_value(arity),
            navigator(navigator) {
    }

    template <typename Alloc>
    tree_base(node_type* root, size_type size, size_type arity, navigator_type navigator, Alloc&& allocator) :
            root(root),
            size_value(size),
            arity_value(arity),
            navigator(navigator),
            allocator(allocator) {
    }

    tree_base(navigator_type navigator, const node_allocator_type& allocator) :
            navigator(navigator),
            allocator(allocator) {
    }

    ~tree_base() {
    }

    /*   ---   ITERATOR METHODS   ---   */
    public:
    /**
     * @brief Returns a constant iterator to the beginning.
     * @details The iterator will point to the first element of the container. Which one is the first depends on the
     * algorithm used to traverse the tree. Using this iterator you can't modify the container.
     * @return constant iterator to the first element
     */
    template <typename P = Policy>
    const_iterator<P> begin(P policy = P()) const {
        return this->cbegin(policy);
    }

    template <typename P = Policy>
    const_iterator<P> cbegin(P policy = P()) const {
        // Incremented to shift it to the first element (initially it's end-equivalent)
        return ++const_iterator<P>(policy, *this);
    }

    template <typename P = Policy>
    const_iterator<P> end(P policy = P()) const {
        return this->cend(policy);
    }

    template <typename P = Policy>
    const_iterator<P> cend(P policy = P()) const {
        return const_iterator<P>(policy, *this);
    }

    template <typename P = Policy>
    const_reverse_iterator<P> rbegin(P policy = P()) const {
        return this->crbegin(policy);
    }

    template <typename P = Policy>
    const_reverse_iterator<P> crbegin(P policy = P()) const {
        return std::make_reverse_iterator(this->cend(policy));
    }

    template <typename P = Policy>
    const_reverse_iterator<P> rend(P policy = P()) const {
        return this->crend(policy);
    }

    template <typename P = Policy>
    const_reverse_iterator<P> crend(P policy = P()) const {
        // Incremented to shift it to the first element (initially it's end-equivalent)
        return std::make_reverse_iterator(this->cbegin(policy));
    }

    template <typename OtherPolicy, bool Constant>
    bool is_own_iterator(
        const tree_iterator<
            tree_base<Node, Policy, Allocator>,
            OtherPolicy,
            Constant>& it) const {
        return it.pointed_tree == this;
    }

    /*   ---   CAPACITY METHODS   ---   */
    public:
    /**
     * @brief Checks whether the container is empty.
     * @details If a tree is empty then:
     * <ul>
     *     <li>It doesn't have a root.</li>
     *     <li>It has a {@link #size()} of 0.</li>
     *     <li>{@link tree#begin() begin()} == {@link tree#end() end()}.</li>
     * </ul>
     * @return true if the tree is empty
     */
    bool empty() const {
        return this->root == nullptr;
    }

    /**
     * @brief Returns the number of the nodes in this tree
     * @return the number of nodes
     */
    size_type size() const {
        if (!this->empty() && this->size_value == 0u) {
            this->size_value = calculate_size(*this->root);
        }
        return this->size_value;
    }

    size_type arity() const {
        if (!this->empty() && this->arity_value == 0u && this->root->has_children()) {
            this->arity_value = calculate_arity(
                *this->root,
                std::is_same_v<std::decay_t<Node>, binary_node<value_type>>
                    ? 2u
                    : std::numeric_limits<std::size_t>::max());
        }
        return this->arity_value;
    }

    /**
     * @brief Returns the maximum possible number of elements the tree can hold.
     * @details Please note that this is not the real value because the size of the tree will be more likely limited by
     * the amount of free memory.
     * @return maximum possible number of elements
     */
    size_type max_size() const {
        return std::numeric_limits<size_type>::max();
    }

    /*   ---   GETTER METHODS   ---   */
    node_type* get_raw_root() const {
        return this->root;
    }

    node_type* get_raw_root() {
        return this->root;
    }

    const_iterator<policy::fixed> get_root() const {
        return {policy::fixed(), *this, this->root};
    }

    node_navigator<const node_type> get_node_navigator() const {
        return this->navigator;
    }

    node_navigator<node_type> get_node_navigator() {
        return this->navigator;
    }

    /// @brief Returns a copy of a useless allocator. (It is an allocator that allocates tree::value_type)
    allocator_type get_allocator() const {
        return this->useless_allocator;
    }

    /// @brief Returns a copy of the object used to allocate/deallocate nodes for this tree.
    node_allocator_type get_node_allocator() const {
        return this->allocator;
    }

    /*  ---   COMPARISON   ---   */
    public:
    template <typename OtherPolicy>
    bool operator==(const tree_base<Node, OtherPolicy, Allocator>& other) const {
        // Trivial test
        if (this->empty() != other.empty()
            || this->size() != other.size()
            || this->arity() != other.arity()) {
            return false;
        }
        // At the end is either null (both) or same as the other
        return (this->root == nullptr && other.root == nullptr) || this->root->operator==(*other.root);
    }

    template <
        typename ConvertibleV,
        typename... Children,
        typename = std::enable_if_t<std::is_convertible_v<ConvertibleV, value_type>>>
    bool operator==(const struct_node<ConvertibleV, Children...>& other) const {
        // Test if different size or arity
        if (this->size() != other.get_subtree_size()
            || this->arity() != other.get_subtree_arity()) {
            return false;
        }
        // Deep test for equality
        return this->root != nullptr && this->root->operator==(other);
    }

    bool operator==(const struct_node<detail::empty_t>&) const {
        return this->empty();
    }
};

// TODO: replace as soon as space ship operator (<=>) is available
// tree
template <
    typename Node,
    typename Policy1,
    typename Policy2,
    typename Allocator>
bool operator!=(const tree_base<Node, Policy1, Allocator>& lhs, const tree_base<Node, Policy2, Allocator>& rhs) {
    return !lhs.operator==(rhs);
}

// struct_node
template <
    typename Node,
    typename Policy,
    typename Allocator,
    typename ConvertibleT,
    typename... Children,
    typename = std::enable_if_t<std::is_convertible_v<ConvertibleT, node_value_t<Node>>>>
bool operator==(const struct_node<ConvertibleT, Children...>& lhs, const tree_base<Node, Policy, Allocator>& rhs) {
    return rhs.operator==(lhs);
}
template <
    typename Node,
    typename Policy,
    typename Allocator,
    typename ConvertibleT,
    typename... Children,
    typename = std::enable_if_t<std::is_convertible_v<ConvertibleT, node_value_t<Node>>>>
bool operator!=(const tree_base<Node, Policy, Allocator>& lhs, const struct_node<ConvertibleT, Children...>& rhs) {
    return !lhs.operator==(rhs);
}
template <
    typename Node,
    typename Policy,
    typename Allocator,
    typename ConvertibleT,
    typename... Children,
    typename = std::enable_if_t<std::is_convertible_v<ConvertibleT, node_value_t<Node>>>>
bool operator!=(const struct_node<ConvertibleT, Children...>& lhs, const tree_base<Node, Policy, Allocator>& rhs) {
    return !rhs.operator==(lhs);
}

// empty struct_node
template <typename Node, typename Policy, typename Allocator>
bool operator==(const struct_node<detail::empty_t>& lhs, const tree_base<Node, Policy, Allocator>& rhs) {
    return rhs.operator==(lhs);
}

template <typename Node, typename Policy, typename Allocator>
bool operator!=(const tree_base<Node, Policy, Allocator>& lhs, const struct_node<detail::empty_t>& rhs) {
    return !lhs.operator==(rhs);
}

template <typename Node, typename Policy, typename Allocator>
bool operator!=(const struct_node<detail::empty_t>& lhs, const tree_base<Node, Policy, Allocator>& rhs) {
    return !rhs.operator==(lhs);
}

template <
    typename Node,
    typename Policy,
    typename Allocator,
    typename = std::enable_if<is_printable<decltype(std::declval<Node>().get_value())>>>
std::ostream& operator<<(std::ostream& os, const tree_base<Node, Policy, Allocator>& tree) {
    if (tree.get_raw_root()) {
        print_node(os, *tree.get_raw_root(), 0u);
    } else {
        code_like_print(os);
    }
    return os;
}

} // namespace md
