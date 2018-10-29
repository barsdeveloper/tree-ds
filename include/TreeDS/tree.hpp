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

namespace ds {

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
 * @tparam Algorithm default iterator algorithm used to traverse the tree in a range-based for loop
 * @tparam Allocator allocator type used to allocate new nodes
 *
 * @sa binary_tree
 */
template <
    typename T,
    typename Node,
    typename Algorithm,
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
    using algorithm_type  = Algorithm;
    using allocator_type  = typename std::allocator_traits<Allocator>::template rebind_alloc<node_type>;

    // Iterators
    template <typename A>
    using iterator = tree_iterator<tree, A, false>;
    template <typename A>
    using const_iterator = tree_iterator<tree, A, true>;
    template <typename A>
    using reverse_iterator = std::reverse_iterator<iterator<A>>;
    template <typename A>
    using const_reverse_iterator = std::reverse_iterator<const_iterator<A>>;

    protected:
    //   ---   ATTRIBUTES   ---
    /// @brief Allocator object used to allocate the nodes.
    allocator_type allocator{};
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

    template <typename OtherPolicy, typename OtherAllocator, CHECK_COPIABLE(T)>
    explicit tree(const tree<T, Node, OtherPolicy, OtherAllocator>& other) :
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
     * Move constructor. Create a tree by movings the tree passed as argument. The tree passed as argument will be empty
     * after this operation.
     * @param other the tree to be moved
     */
    template <typename OtherPolicy>
    tree(tree<T, Node, OtherPolicy, Allocator>&& other) :
            allocator(other.allocator),
            root(other.root),
            size_value(other.size_value) {
        other.emptify();
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
    template <typename OtherPolicy, typename OtherAllocator>
    tree& operator=(const tree<T, Node, OtherPolicy, OtherAllocator>& other) {
        static_assert(
            std::is_nothrow_copy_assignable_v<T>,
            "Tried to COPY ASSIGN a tree containing a non copyable type.");
        this->assign(
            allocate(allocator, *other.root, allocator).release(),
            other.size);
        return *this;
    }

    template <typename OtherPolicy>
    tree& operator=(tree<T, Node, OtherPolicy, Allocator>&& other) {
        allocator = other.allocator;
        this->assign(other.root, other.size_value);
        other.emptify();
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
    template <typename A = Algorithm>
    iterator<A> begin() {
        // Incremented to shift it to the first element (initially it's end-equivalent)
        return ++iterator<A>(*this);
    }

    /**
     * @brief Returns a constant iterator to the beginning.
     * @details The iterator will point to the first element of the container. Which one is the first depends on the
     * algorithm used to traverse the tree. Using this iterator you can't modify the container.
     * @return constant iterator to the first element
     */
    template <typename A = Algorithm>
    const_iterator<A> begin() const {
        return cbegin();
    }

    /**
     * @copydoc #begin() const
     */
    template <typename A = Algorithm>
    const_iterator<A> cbegin() const {
        // Incremented to shift it to the first element (initially it's end-equivalent)
        return ++const_iterator<A>(*this);
    }

    /**
     * @brief Returns an iterator to the end.
     *
     * The iterator will point one step after the last element of the container. Which one is the last depends on the
     * algorithm used to traverse the tree. It's perfectly legitimate to decrement this iterator to obtain the last
     * element.
     * @return iterator the element following the last element
     */
    template <typename A = Algorithm>
    iterator<A> end() {
        return iterator<A>(*this);
    }

    template <typename A = Algorithm>
    const_iterator<A> end() const {
        return cend();
    }

    template <typename A = Algorithm>
    const_iterator<A> cend() const {
        return const_iterator<A>(*this);
    }

    template <typename A = Algorithm>
    reverse_iterator<A> rbegin() {
        return std::make_reverse_iterator(end<A>());
    }

    template <typename A = Algorithm>
    const_reverse_iterator<A> rbegin() const {
        return crbegin();
    }

    template <typename A = Algorithm>
    const_reverse_iterator<A> crbegin() const {
        return std::make_reverse_iterator(cend<A>());
    }

    // reverse end
    template <typename A = Algorithm>
    reverse_iterator<A> rend() {
        // Incremented to shift it to the first element (initially it's end-equivalent)
        return std::make_reverse_iterator(begin<A>());
    }

    template <typename A = Algorithm>
    const_reverse_iterator<A> rend() const {
        return crend();
    }

    template <typename A = Algorithm>
    const_reverse_iterator<A> crend() const {
        // Incremented to shift it to the first element (initially it's end-equivalent)
        return std::make_reverse_iterator(cbegin<A>());
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

    //   ---   MODIFIERS   ---
    protected:
    template <typename A, bool Constant>
    tree_iterator<tree, A, Constant>
    modify_subtree(
        tree_iterator<tree, A, Constant> position,
        std::unique_ptr<node_type, deleter<allocator_type>> node,
        std::size_t replacement_size) {
        if (position.pointed_tree != this) {
            throw std::logic_error("Tried to modify the tree (insert or emplace) with an iterator not belonging to.");
        }
        node_type* replacement = node.release();
        node_type* target      = const_cast<node_type*>(position.get_node());
        if (target != nullptr) { // if iterator points to valid node
            assert(root != nullptr);
            position.update(*target, *replacement);
            if (target == root) {
                assign(replacement, replacement_size);
            } else {
                target->replace_with(*replacement);
                size_value -= count_nodes(*target);
                deallocate(allocator, target);
            }
        } else if (root == nullptr) {
            root = replacement;
        } else {
            throw std::logic_error("The iterator points to a non valid position (end).");
        }
        size_value += replacement_size;
        return position;
    }

    void emptify() {
        root       = nullptr;
        size_value = 0u;
    }

    void assign(node_type* root, std::size_t size) {
        if (this->root != nullptr) {
            deallocate(this->allocator, this->root);
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
    void swap(tree& other) {
        // I want unqualified name lookup in order to let invoking an user-defined swap function outside std namespace.
        using namespace std;
        swap(root, other.root);
        swap(size_value, other.size_value);
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
        return modify_subtree(position, allocate(allocator, node, allocator), node.get_subtree_size());
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
        return modify_subtree(position, allocate(allocator, value), 1u);
    }

    /**
     * Insert an element at the specified position by replacing the existent node. This will use the move semantics.
     * @param position where to insert the element
     * @param value to insert in the tree, that will be copied
     * @return an iterator that points to the inserted element
     */
    template <typename It>
    It insert(It position, T&& value) {
        return modify_subtree(position, allocate(allocator, std::move(value)), 1u);
    }

    template <typename It, typename... Args>
    It emplace(It position, Args&&... args) {
        return modify_subtree(position, allocate(allocator, std::forward<Args>(args)...), 1u);
    }

    template <
        typename It,
        typename... EmplacingArgs,
        typename... Children,
        CHECK_CONSTRUCTIBLE(value_type, EmplacingArgs...)>
    It emplace(It position, const struct_node<std::tuple<EmplacingArgs...>, Children...>& nodes) {
        return modify_subtree(position, allocate(allocator, nodes, allocator), 1u);
    }

    iterator<post_order> erase(iterator<post_order> position) {
        node_type* node = position++.get_node();
        assert(node != nullptr);
        deallocate(allocator, node);
        return position;
    }

    iterator<post_order> erase(iterator<post_order> first, iterator<post_order> last) {
        while (first != last) {
            node_type* node = first++.get_node();
            assert(node != nullptr);
            deallocate(allocator, node);
        }
        return first;
    }

    //  ---   COMPARISON   ---
    template <typename OtherPolicy, typename OtherAllocator>
    bool operator==(const tree<T, Node, OtherPolicy, OtherAllocator>& other) const {
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
};

template <
    typename T,
    typename Node,
    typename Policy1,
    typename Policy2,
    typename Allocator1,
    typename Allocator2>
bool operator!=(const tree<T, Node, Policy1, Allocator1>& lhs, const tree<T, Node, Policy2, Allocator2>& rhs) {
    return !lhs.operator==(rhs);
}

} // namespace ds
