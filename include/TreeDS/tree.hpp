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
    /// @brief Owning pointer to the root node.
    std::unique_ptr<node_type, deleter<allocator_type>> root{};
    /// @brief The number of nodes in the tree.
    size_type size_value = 0u;
    allocator_type allocator{};

    protected:
    template <typename ConvertibleNode>
    tree(const ConvertibleNode* root_ptr, size_type size) :
            root(
                root_ptr != nullptr
                    ? std::move(allocate(allocator, *root_ptr, allocator))
                    : nullptr),
            size_value(size) {
    }

    public:
    //   ---   METHODS   ---
    /**
     * Create an empty tree.
     */
    constexpr tree() = default;

    /**
     * Copy constructor. Create a tree by copying the tree passed as argument. Remember that this involves deep copy
     * (use std::move() whenever you can). A new {@link node_type} will be allocated for each node of the other tree.
     * The allocator will be used.
     * @param other the tree to be copied from
     */
    tree(const tree& other) :
            root(
                other.root
                    ? std::move(allocate(allocator, *other.root, allocator))
                    : decltype(root){}),
            size_value(other.size_value) {
    }

    template <typename OtherPolicy, typename OtherAllocator>
    tree(const tree<T, Node, OtherPolicy, OtherAllocator>& other) :
            root(
                other.root
                    ? std::move(allocate(allocator, *other.root, allocator))
                    : decltype(root){}),
            size_value(other.size_value) {
    }

    /**
     * Move constructor. Create a tree by movings the tree passed as argument. The tree passed as argument will be empty
     * after this operation.
     * @param other the tree to be moved
     */
    template <typename OtherPolicy, typename OtherAllocator>
    tree(tree<T, Node, OtherPolicy, OtherAllocator>&& other) :
            root(std::move(other.root)),
            size_value(other.size_value) {
        other.nullify();
    }

    /**
     * Create a tree by copying the nodes structure starting from the {@link temporary_node} passed as argument. A new
     * {@link node_type} will be allocated for each node in the structure passed. The allocation will use the allocator.
     * @param root the root of the newly created tree
     */
    template <
        typename ConvertibleT,
        typename... Children,
        CHECK_CONVERTIBLE(ConvertibleT, value_type)>
    tree(const struct_node<ConvertibleT, Children...>& root) :
            root(std::move(allocate(allocator, root, allocator))),
            size_value(root.get_subtree_size()) {
    }

    /*
     * Destruct all the nodes and deallocate the memory owned by this tree.
     */
    ~tree() = default;

    tree& operator=(const tree& other) {
        this->root = std::move(allocate(allocator, *other.root, allocator));
        this->size = other.size;
        return *this;
    }

    tree& operator=(tree&& other) {
        this->root       = std::move(other.root);
        this->size_value = other.size_value;
        other.nullify();
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
        return ++iterator<A>(this);
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
        return ++const_iterator<A>(this);
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
        return iterator<A>(this);
    }

    template <typename A = Algorithm>
    const_iterator<A> end() const {
        return cend();
    }

    template <typename A = Algorithm>
    const_iterator<A> cend() const {
        return const_iterator<A>(this);
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
        return root == nullptr;
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
        return root.get();
    }

    //   ---   MODIFIERS   ---
    protected:
    void nullify() {
        root       = nullptr;
        size_value = 0u;
    }

    template <typename A, bool Constant>
    tree_iterator<tree, A, Constant>
    insert(tree_iterator<tree, A, Constant> position, decltype(root) node, std::size_t replacement_size) {
        if (position.pointed_tree != this) {
            throw std::logic_error("The iterator passed is not associated with tree");
        }
        size_value += replacement_size;
        node_type* target = const_cast<node_type*>(position.get_node());
        if (target != nullptr) { // if iterator points to valid node
            node_type* replacement = node.release();
            target->replace_with(*replacement);
            size_value -= count_nodes(*target);
            position.update(*target, *replacement);
        } else if (root == nullptr) {
            root = std::move(node);
        } else {
            throw std::logic_error("The iterator points to a non valid position (end()).");
        }
        return position;
    }

    public:
    /**
     * @brief Removes all elements from the tree.
     * @details Invalidates any references, pointers, or iterators referring to contained elements. Any past-the-last
     * element iterator ({@link tree#end() end()}) remains valid.
     */
    void clear() {
        this->nullify();
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
        return insert(position, allocate(allocator, node), node.get_subtree_size());
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
        return insert(position, allocate(allocator, value, 1u));
    }

    /**
     * Insert an element at the specified position by replacing the existent node. This will use the move semantics.
     * @param position where to insert the element
     * @param value to insert in the tree, that will be copied
     * @return an iterator that points to the inserted element
     */
    template <typename It>
    It insert(It position, T&& value) {
        return insert(position, allocate(allocator, std::move(value)), 1u);
    }

    template <typename It, typename... Args>
    It emplace(It position, Args&&... args) {
        return insert(position, allocate(allocator, std::forward<Args>(args)...), 1u);
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
bool operator!=(const tree<T, Node, Policy1, Allocator1> lhs, const tree<T, Node, Policy2, Allocator2>& rhs) {
    return !lhs.operator==(rhs);
}

} // namespace ds
