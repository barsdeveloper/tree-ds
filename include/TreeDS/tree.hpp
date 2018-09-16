#pragma once

#include <functional>  // std::bind(), std::mem_fun()
#include <limits>      // std::numeric_limits()
#include <memory>      // std::unique_ptr, std::allocator_traits
#include <stdexcept>   // std::logic_error
#include <tuple>       // make_from_tuple
#include <type_traits> // std::enable_if
#include <utility>     // std::move(), std::forward()

#include <TreeDS/allocator_utility.hpp>
#include <TreeDS/binary_tree.hpp>
#include <TreeDS/iterator/in_order.hpp>
#include <TreeDS/iterator/post_order.hpp>
#include <TreeDS/iterator/pre_order.hpp>
#include <TreeDS/node/binary_node.hpp>
#include <TreeDS/node/struct_node.hpp>
#include <TreeDS/tree_iterator.hpp>
#include <TreeDS/utility.hpp>

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
    template <typename> class Node,
    typename Algorithm,
    typename Allocator>
class tree {

    template <typename, template <typename> class, typename, typename>
    friend class tree;

    template <typename, typename, bool>
    friend class tree_iterator;

    public:
    /*   ---   Types declarations   ---   */

    // General
    using value_type      = T;
    using reference       = value_type&;
    using const_reference = const value_type&;
    using node_type       = Node<T>;
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
    allocator_type allocator{};
    /// @brief Owning pointer to the root node.
    std::unique_ptr<node_type, deleter<allocator_type>> root{};
    /// @brief The number of nodes in the tree.
    size_type size_value = 0u;

    public:
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

    /**
     * Move constructor. Create a tree by movings the tree passed as argument. The tree passed as argument will be empty
     * after this operation.
     * @param other the tree to be moved
     */
    tree(tree&& other) :
            root(std::move(other.root)),
            size_value(other.size_value) {
        other.nullify();
    }

    /*
     * Destruct all the nodes and deallocate the memory owned by this tree.
     */
    ~tree() = default;

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

    /*   ---   Iterators   ---   */
    /**
     * @brief Returns an iterator to the beginning.
     * @details The iterator will point to the first element of the container. Which one is the first depends on the
     * algorithm used to traverse the tree.
     * @return iterator to the first element
     */
    template <typename A = algorithm_type>
    iterator<A> begin() {
        // Incremented to get it to the first element
        return ++iterator<A>(this);
    }

    /**
     * @brief Returns a constant iterator to the beginning.
     * @details The iterator will point to the first element of the container. Which one is the first depends on the
     * algorithm used to traverse the tree. Using this iterator you can't modify the container.
     * @return constant iterator to the first element
     */
    template <typename A = algorithm_type>
    const_iterator<A> begin() const {
        return cbegin();
    }

    /**
     * @copydoc #begin() const
     */
    template <typename A = algorithm_type>
    const_iterator<A> cbegin() const {
        // Incremented to get it to the first element
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
    template <typename A = algorithm_type>
    iterator<A> end() {
        return iterator<A>(this);
    }

    template <typename A = algorithm_type>
    const_iterator<A> end() const {
        return cend();
    }

    template <typename A = algorithm_type>
    const_iterator<A> cend() const {
        return const_iterator<A>(this);
    }

    template <typename A = algorithm_type>
    reverse_iterator<A> rbegin() {
        return reverse_iterator<A>(iterator<A>(this));
    }

    template <typename A = algorithm_type>
    const_reverse_iterator<A> rbegin() const {
        /*
         * Why we don'_tree increment the reverse_iterator as we do with iterator?
         * It would make sense: when constructed the iterator is at the end() (which is same for both iterator and
         * reverse_iterator) incrementing will result in an actual decrementing taking the iterator to the last element
         * (that is reverse_iterator's fist element). We don't need to do this because a reverse_iterator will be
         * decremented before dereferenced (every time).
         * See "http://en.cppreference.com/w/cpp/iterator/reverse_iterator/operator*".
         * This seems like a waste of resources to me but it works that way.
         * In general reverse_iterator == iterator - 1
         */
        return crbegin();
    }

    template <typename A = algorithm_type>
    const_reverse_iterator<A> crbegin() const {
        return const_reverse_iterator<A>(const_iterator<A>(this));
    }

    // reverse end
    template <typename A = algorithm_type>
    reverse_iterator<A> rend() {
        // reverse_iterator takes an iterator as argument, we construct it using {this}
        return --reverse_iterator<A>(iterator<A>(this));
    }

    template <typename A = algorithm_type>
    const_reverse_iterator<A> rend() const {
        return crend();
    }

    template <typename A = algorithm_type>
    const_reverse_iterator<A> crend() const {
        // reverse_iterator takes an iterator as argument, we construcit using {this}
        return --const_reverse_iterator<A>(const_iterator<A>(this));
    }

    /*   ---   Capacity   ---   */
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

    /*   ---   Modifiers   ---   */
    protected:
    void nullify() {
        root       = nullptr;
        size_value = 0u;
    }

    template <typename It>
    iterator<typename It::algorithm_type>
    insert(It position, decltype(root) node) {
        node_type* target = position.get_node();
        if (target) { // if iterator points to valid node
            target->move_resources_to(*node);
        } else if (!this->root) {
            this->root = std::move(node);
        } else {
            throw std::logic_error("Tried to insert node in a not valid position.");
        }
        return {this, node.get()};
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
    iterator<typename It::algorithm_type>
    insert(It position, const struct_node<ConvertibleT, Children...>& node) {
        this->size_value += node.get_subtree_size() - 666; // TODO perform calculation
        return insert(position, std::make_unique<node_type>(node));
    }

    /**
     * Insert an element at the specified position by replacing the existent
     * node. This will use the copy semantics.
     * @param position where to insert the element
     * @param value to insert in the tree, that will be copied
     * @return an iterator that points to the inserted element
     */
    template <typename It>
    iterator<typename It::algorithm_type> insert(It position, const_reference value) {
        ++this->size_value;
        return insert(position, std::make_unique<node_type>(value));
    }

    /**
     * Insert an element at the specified position by replacing the existent node. This will use the move semantics.
     * @param position where to insert the element
     * @param value to insert in the tree, that will be copied
     * @return an iterator that points to the inserted element
     */
    template <typename It>
    iterator<typename It::algorithm_type> insert(It position, value_type&& value) {
        ++this->size_value;
        return insert(position, allocate(allocator, std::move(value)));
    }

    template <typename It, typename... Args>
    iterator<typename It::algorithm_type> emplace(It position, Args&&... args) {
        ++this->size_value;
        return insert(position, allocate(allocator, std::forward<Args>(args)...));
    }

    /*   ---   Equality comparison   ---   */
    bool operator==(const tree& other) const {
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

} // namespace ds
