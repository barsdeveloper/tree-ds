#pragma once

#include <functional>  // std::bind(), std::mem_fun()
#include <iterator>    // std::make_reverse_iterator
#include <limits>      // std::numeric_limits()
#include <memory>      // std::unique_ptr, std::allocator_traits
#include <stdexcept>   // std::logic_error
#include <tuple>       // make_from_tuple
#include <type_traits> // std::enable_if
#include <utility>     // std::move(), std::forward()

#include <TreeDS/node/struct_node.hpp>
#include <TreeDS/policy/post_order.hpp>
#include <TreeDS/tree_iterator.hpp>

namespace md {

/**
 * @brief A generic tree class that uses a custom node type.
 * @details A binary tree structured as a hierarchy of {@link node}. Every node
 * keeps an object of type T and can have a most two nodes as children. This
 * class is compatible with STL containers. For example you can insert an
 * element into the data structure by calling the usual:<br />
 * <ul>
 *     <li>{@link #insert(It, const_reference) insert(iterator, const T&)} - if you want copy semantics</li>
 *     <li>{@link #insert(It, value_type&&) insert(iterator, T&&)} - if you want move semantics</li>
 * </ul>
 * method and using an {@link #iterator} as place indicator.
 *
 * @tparam T type of element hold by the tree
 * @tparam Policy default iterator algorithm used to traverse the tree in a range-based for loop
 * @tparam Allocator allocator type used to allocate new nodes
 *
 * @sa binary_tree
 */
template <
    typename T,
    typename Node,
    typename Policy,
    typename Allocator>
class tree {

    template <typename, typename, typename, typename>
    friend class tree;

    template <typename, typename, bool>
    friend class tree_iterator;

    public:
    //   ---   TYPES   ---

    // General
    using value_type      = T;
    using reference       = value_type&;
    using const_reference = const value_type&;
    using node_type       = Node;
    using size_type       = std::size_t;
    using difference_type = std::ptrdiff_t;
    using pointer         = typename std::allocator_traits<Allocator>::pointer;
    using const_pointer   = typename std::allocator_traits<Allocator>::const_pointer;
    using policy_type     = Policy;
    using allocator_type  = typename std::allocator_traits<Allocator>::template rebind_alloc<node_type>;

    // Iterators
    template <typename P>
    using iterator = tree_iterator<tree, P, false>;
    template <typename P>
    using const_iterator = tree_iterator<tree, P, true>;
    template <typename P>
    using reverse_iterator = std::reverse_iterator<iterator<P>>;
    template <typename P>
    using const_reverse_iterator = std::reverse_iterator<const_iterator<P>>;

    protected:
    //   ---   ATTRIBUTES   ---
    /// @brief Allocator object used to allocate the nodes.
    allocator_type allocator {};
    /// @brief Owning pointer to the root node.
    node_type* root = nullptr;
    /// @brief The number of nodes in the tree.
    size_type size_value = 0u;

    protected:
    tree(node_type* root, size_type size) :
            root(root),
            size_value(size) {
    }

    public:
    //   ---   CONSTRUCTORS   ---
    /**
     * Create an empty tree.
     */
    constexpr tree() = default;

    explicit tree(const Allocator& allocator) :
            allocator(allocator) {
    }

    /**
     * Copy constructor. Create a tree by copying the tree passed as argument. Remember that this involves deep copy
     * (use std::move() whenever you can). A new {@link node_type} will be allocated for each node of the other tree.
     * The allocator will be used.
     * @param other the tree to be copied from
     */
    explicit tree(const tree& other) :
            allocator(other.allocator),
            root(
                other.root
                    ? allocate(allocator, *other.root, allocator).release()
                    : nullptr),
            size_value(other.size_value) {
        static_assert(
            std::is_copy_constructible_v<T>,
            "Tried to COPY a tree containing a non copyable type.");
    }

    template <typename OtherPolicy>
    explicit tree(const tree<T, Node, OtherPolicy, Allocator>& other) :
            root(
                other.root
                    ? allocate(allocator, *other.root, allocator).release()
                    : nullptr),
            size_value(other.size_value) {
        static_assert(
            std::is_copy_constructible_v<T>,
            "Tried to COPY a tree containing a non copyable type.");
    }

    /**
     * Other Policy move constructor. Create a tree by movings the tree passed as argument. The tree passed as argument will be empty
     * after this operation.
     * @param other the tree to be moved
     */
    template <typename OtherPolicy>
    tree(tree<T, Node, OtherPolicy, Allocator>&& other) :
            allocator(std::move(other.allocator)),
            root(other.root),
            size_value(other.size_value) {
        other.root       = nullptr;
        other.size_value = 0u;
    }

    /**
     * Create a tree by copying the nodes structure starting from the {@link temporary_node} passed as argument. A new
     * {@link node_type} will be allocated for each node in the structure passed. The allocation will use the allocator.
     * @param root the root of the newly created tree
     */
    template <
        typename ConvertibleT,
        typename... Children,
        CHECK_CONVERTIBLE(ConvertibleT, T)>
    tree(const struct_node<ConvertibleT, Children...>& root) :
            root(allocate(allocator, root, allocator).release()),
            size_value(root.get_subtree_size()) {
    }

    template <
        typename... EmplacingArgs,
        typename... Children,
        CHECK_CONSTRUCTIBLE(value_type, EmplacingArgs...)>
    tree(const struct_node<std::tuple<EmplacingArgs...>, Children...>& root) :
            root(allocate(allocator, root, allocator).release()),
            size_value(root.get_subtree_size()) {
    }

    /*
     * Destruct all the nodes and deallocate the memory owned by this tree.
     */
    ~tree() {
        clear();
    }

    public:
    //   ---   ASSIGNMENT   ---
    // Copy assignment
    tree& operator=(const tree& other) {
        return this->operator=<Policy, Allocator>(other);
    }

    // Any other policy copy assignment
    template <typename OtherPolicy>
    tree& operator=(const tree<T, Node, OtherPolicy, Allocator>& other) {
        static_assert(
            std::is_nothrow_copy_assignable_v<T>,
            "Tried to COPY ASSIGN a tree containing a non copyable type.");
        this->assign(
            !other.empty()
                ? allocate(allocator, *other.root, allocator).release()
                : nullptr,
            other.size());

        return *this;
    }

    // Move assignment
    tree& operator=(tree&& other) {
        return this->operator=<Policy>(other);
    }

    // Any other policy move assignment
    template <typename OtherPolicy>
    tree& operator=(tree<T, Node, OtherPolicy, Allocator>&& other) {
        allocator = std::move(other.allocator);
        this->assign(other.root, other.size_value);
        other.root       = nullptr;
        other.size_value = 0u;
        return *this;
    }

    template <
        typename ConvertibleT,
        typename... Nodes,
        CHECK_CONVERTIBLE(ConvertibleT, T)>
    tree& operator=(const struct_node<ConvertibleT, Nodes...>& root) {
        this->assign(
            allocate(allocator, root, allocator).release(),
            root.get_subtree_size());
        return *this;
    }

    template <
        typename... EmplacingArgs,
        typename... Children,
        CHECK_CONSTRUCTIBLE(value_type, EmplacingArgs...)>
    tree& operator=(const struct_node<std::tuple<EmplacingArgs...>, Children...>& root) {
        this->assign(
            allocate(allocator, root, allocator).release(),
            root.get_subtree_size());
        return *this;
    }

    //   ---   ITERATORS   ---
    /**
     * @brief Returns an iterator to the beginning.
     * @details The iterator will point to the first element of the container. Which one is the first depends on the
     * algorithm used to traverse the tree.
     * @return iterator to the first element
     */
    template <typename P = Policy>
    iterator<P> begin() {
        // Incremented to shift it to the first element (initially it's end-equivalent)
        return ++iterator<P>(*this);
    }

    /**
     * @brief Returns a constant iterator to the beginning.
     * @details The iterator will point to the first element of the container. Which one is the first depends on the
     * algorithm used to traverse the tree. Using this iterator you can't modify the container.
     * @return constant iterator to the first element
     */
    template <typename P = Policy>
    const_iterator<P> begin() const {
        return cbegin();
    }

    /**
     * @copydoc #begin() const
     */
    template <typename P = Policy>
    const_iterator<P> cbegin() const {
        // Incremented to shift it to the first element (initially it's end-equivalent)
        return ++const_iterator<P>(*this);
    }

    /**
     * @brief Returns an iterator to the end.
     *
     * The iterator will point one step after the last element of the container. Which one is the last depends on the
     * algorithm used to traverse the tree. It's perfectly legitimate to decrement this iterator to obtain the last
     * element.
     * @return iterator the element following the last element
     */
    template <typename P = Policy>
    iterator<P> end() {
        return iterator<P>(*this);
    }

    template <typename P = Policy>
    const_iterator<P> end() const {
        return cend<P>();
    }

    template <typename P = Policy>
    const_iterator<P> cend() const {
        return const_iterator<P>(*this);
    }

    template <typename P = Policy>
    reverse_iterator<P> rbegin() {
        return std::make_reverse_iterator(end<P>());
    }

    template <typename P = Policy>
    const_reverse_iterator<P> rbegin() const {
        return crbegin<P>();
    }

    template <typename P = Policy>
    const_reverse_iterator<P> crbegin() const {
        return std::make_reverse_iterator(cend<P>());
    }

    // reverse end
    template <typename P = Policy>
    reverse_iterator<P> rend() {
        // Incremented to shift it to the first element (initially it's end-equivalent)
        return std::make_reverse_iterator(begin<P>());
    }

    template <typename P = Policy>
    const_reverse_iterator<P> rend() const {
        return crend<P>();
    }

    template <typename P = Policy>
    const_reverse_iterator<P> crend() const {
        // Incremented to shift it to the first element (initially it's end-equivalent)
        return std::make_reverse_iterator(cbegin<P>());
    }

    public:
    allocator_type get_allocator() const {
        return allocator;
    }

    //   ---   CAPACITY   ---
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
        return size_value == 0;
    }

    /**
     * @brief Returns the number of the nodes in this tree
     * @return the number of nodes
     */
    size_type size() const {
        return size_value;
    }

    /**
     * @brief Returns the maximum possible number of elements the tree can hold.
     * @details Please note that this is not the real value because the size of the tree will be more likely limited by
     * the amount of free memory. You never want to use this method, it is stupid, it exists just to say that this
     * library is stl-like.
     * @return maximum possible number of elements
     */
    size_type max_size() const {
        return std::numeric_limits<size_type>::max();
    }

    const Node* get_root() const {
        return root;
    }

    Node* get_root() {
        return root;
    }

    //   ---   MODIFIERS   ---
    protected:
    void replace_node(node_type* replaced, node_type* replacement) {
        replaced->replace_with(replacement);
        this->size_value -= count_nodes(*replaced);
        deallocate(this->allocator, replaced);
    }

    template <typename P, bool Constant>
    tree_iterator<tree, P, Constant>
    modify_subtree(
        tree_iterator<tree, P, Constant> position,
        std::unique_ptr<node_type, deleter<allocator_type>> node,
        std::size_t replacement_size) {
        if (position.pointed_tree != this) {
            throw std::logic_error("Tried to modify the tree (insert or emplace) with an iterator not belonging to it.");
        }
        node_type* replacement = node.release();
        node_type* target      = const_cast<node_type*>(position.get_node());
        if (target != nullptr) { // if iterator points to valid node
            assert(root != nullptr);
            position.update(*target, replacement);
            if (target == root) {
                this->assign(replacement, 0); // size will be incremented at the end
            } else {
                this->replace_node(target, replacement);
            }
        } else if (root == nullptr) {
            root = replacement;
        } else {
            throw std::logic_error("The iterator points to a non valid position (end).");
        }
        size_value += replacement_size;
        return position;
    }

    template <typename P, bool Constant>
    void erase_subtree(tree_iterator<tree, P, Constant> position) {
        node_type* target = const_cast<node_type*>(position.get_node());
        if (target != nullptr) { // if iterator points to valid node
            assert(root != nullptr);
            position.update(*target, nullptr);
            if (target == root) {
                this->clear();
            } else {
                this->replace_node(target, nullptr);
            }
        } else if (root != nullptr) {
            throw std::logic_error("The iterator points to a non valid position (end).");
        }
    }

    void assign(node_type* root, std::size_t size) {
        if (this->root != nullptr) {
            deallocate(this->allocator, this->root);
            this->size_value = 0u;
        }
        this->root       = root;
        this->size_value = size;
    }

    public:
    /**
     * @brief Removes all elements from the tree.
     * @details Invalidates any references, pointers, or iterators referring to contained elements. Any past-the-last
     * element iterator ({@link tree#end() end()}) remains valid.
     */
    void clear() {
        assign(nullptr, 0u);
    }

    /**
     * @brief Swaps the contents of this tree with those of the other.
     * @details After this method will be called, this tree will have the root and size of the other tree while the
     * other tree will have root and size of this tree. All references to elements remain valid. Iterators however will
     * not. Do not use iterator after you swapped trees.
     * @param other the tree to swap with
     */
    template <typename OtherPolicy>
    void swap(tree<T, Node, OtherPolicy, Allocator>& other) {
        // I want unqualified name lookup in order to let invoking an user-defined swap function outside std namespace.
        using namespace std;
        std::swap(root, other.root);
        std::swap(size_value, other.size_value);
    }

    /**
     * Insert an element at the specified position by replacing the existent
     * node. This will use the copy semantics.
     * @param position where to insert the element
     * @param node an r-value reference to a temporary_node
     * @return an iterator that points to the inserted element
     */
    template <
        typename It,
        typename ConvertibleT,
        typename... Children,
        CHECK_CONVERTIBLE(ConvertibleT, value_type)>
    It insert(It position, const struct_node<ConvertibleT, Children...>& node) {
        return this->modify_subtree(
            position,
            allocate(this->allocator, node, this->allocator),
            node.get_subtree_size());
    }

    /**
     * Insert an element at the specified position by replacing the existent
     * node. This will use the copy semantics.
     * @param position where to insert the element
     * @param value to insert in the tree, that will be copied
     * @return an iterator that points to the inserted element
     */
    template <typename It>
    It insert(It position, const T& value) {
        return this->modify_subtree(position, allocate(allocator, value), 1u);
    }

    /**
     * Insert an element at the specified position by replacing the existent node. This will use the move semantics.
     * @param position where to insert the element
     * @param value to insert in the tree, that will be copied
     * @return an iterator that points to the inserted element
     */
    template <typename It>
    It insert(It position, T&& value) {
        return this->modify_subtree(position, allocate(allocator, std::move(value)), 1u);
    }

    template <
        typename It,
        typename... Args,
        CHECK_CONSTRUCTIBLE(T, Args&&...)>
    It emplace(It position, Args&&... args) {
        return this->modify_subtree(position, allocate(allocator, std::forward<Args>(args)...), 1u);
    }

    template <
        typename It,
        typename... EmplacingArgs,
        typename... Children,
        CHECK_CONSTRUCTIBLE(value_type, EmplacingArgs...)>
    It emplace(It position, const struct_node<std::tuple<EmplacingArgs...>, Children...>& nodes) {
        return this->modify_subtree(position, allocate(allocator, nodes, allocator), nodes.get_subtree_size());
    }

    iterator<post_order> erase(const_iterator<post_order> position) {
        if (position.pointed_tree != this) {
            throw std::logic_error("Tried to modify the tree (erase) with an iterator not belonging to it.");
        }
        iterator<post_order> result(position.craft_non_constant_iterator());
        this->erase_subtree(position);
        return ++result;
    }

    iterator<post_order> erase(const_iterator<post_order> first, const_iterator<post_order> last) {
        if (first.pointed_tree != this || last.pointed_tree != this) {
            throw std::logic_error("Tried to modify the tree (erase) with an iterator not belonging to it.");
        }
        while (first != last) {
            this->erase_subtree(first);
            ++first;
        }
        return ++last.craft_non_constant_iterator();
    }

    //  ---   COMPARISON   ---
    template <typename OtherPolicy>
    bool operator==(const tree<T, Node, OtherPolicy, Allocator>& other) const {
        // 1. Test if different size_value
        if (this->size_value != other.size_value) {
            return false;
        }
        // 2. Test if one between this or other has a root that is set while the other doesn't.
        if ((this->root == nullptr) != (other.root == nullptr)) {
            return false;
        }
        // At the end is either null (both) or same as the other.
        return this->root == nullptr || *this->root == *other.root;
    }

    template <
        typename ConvertibleT,
        typename... Children,
        CHECK_CONVERTIBLE(ConvertibleT, value_type)>
    bool operator==(const struct_node<ConvertibleT, Children...>& other) const {
        // Test if different size_value (trivial case for performance)
        if (this->size_value != other.get_subtree_size()) {
            return false;
        }
        // Deep test for equality
        return root && root->operator==(other);
    }

    bool operator==(const struct_node<std::nullptr_t>&) const {
        return this->empty();
    }
};

// TODO: replace as soon as space ship operator (<=>) is available
// tree
template <
    typename T,
    typename Node,
    typename Policy1,
    typename Policy2,
    typename Allocator>
bool operator!=(const tree<T, Node, Policy1, Allocator>& lhs, const tree<T, Node, Policy2, Allocator>& rhs) {
    return !lhs.operator==(rhs);
}

// struct_node
template <
    typename T,
    typename Node,
    typename Policy,
    typename Allocator,
    typename ConvertibleT,
    typename... Children,
    CHECK_CONVERTIBLE(ConvertibleT, T)>
bool operator==(const struct_node<ConvertibleT, Children...>& lhs, const tree<T, Node, Policy, Allocator>& rhs) {
    return rhs.operator==(lhs);
}
template <
    typename T,
    typename Node,
    typename Policy,
    typename Allocator,
    typename ConvertibleT,
    typename... Children,
    CHECK_CONVERTIBLE(ConvertibleT, T)>
bool operator!=(const tree<T, Node, Policy, Allocator>& lhs, const struct_node<ConvertibleT, Children...>& rhs) {
    return !lhs.operator==(rhs);
}
template <
    typename T,
    typename Node,
    typename Policy,
    typename Allocator,
    typename ConvertibleT,
    typename... Children,
    CHECK_CONVERTIBLE(ConvertibleT, T)>
bool operator!=(const struct_node<ConvertibleT, Children...>& lhs, const tree<T, Node, Policy, Allocator>& rhs) {
    return !rhs.operator==(lhs);
}

// empty struct_node
template <
    typename T,
    typename Node,
    typename Policy,
    typename Allocator>
bool operator==(const struct_node<std::nullptr_t>& lhs, const tree<T, Node, Policy, Allocator>& rhs) {
    return rhs.operator==(lhs);
}

template <
    typename T,
    typename Node,
    typename Policy,
    typename Allocator>
bool operator!=(const tree<T, Node, Policy, Allocator>& lhs, const struct_node<std::nullptr_t>& rhs) {
    return !lhs.operator==(rhs);
}

template <
    typename T,
    typename Node,
    typename Policy,
    typename Allocator>
bool operator!=(const struct_node<std::nullptr_t>& lhs, const tree<T, Node, Policy, Allocator>& rhs) {
    return !rhs.operator==(lhs);
}

// Swappable requirement
template <
    typename T,
    typename Node,
    typename Policy1,
    typename Policy2,
    typename Allocator>
void swap(tree<T, Node, Policy1, Allocator>& lhs, tree<T, Node, Policy2, Allocator>& rhs) noexcept {
    return lhs.swap(rhs);
}

} // namespace md
