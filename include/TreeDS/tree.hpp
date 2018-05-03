#pragma once

#include <TreeDS/binary_node.hpp>
#include <TreeDS/iterator/in_order.hpp>
#include <TreeDS/iterator/post_order.hpp>
#include <TreeDS/iterator/pre_order.hpp>
#include <TreeDS/std_implementations.hpp>
#include <TreeDS/temporary_node.hpp>
#include <TreeDS/tree_iterator.hpp>
#include <TreeDS/utility.hpp>
#include <functional>  // std::bind(), std::mem_fun()
#include <limits>      // std::numeric_limits()
#include <memory>      // std::unique_ptr, std::allocator_traits
#include <stdexcept>   // std::logic_error
#include <tuple>       // make_from_tuple
#include <type_traits> // std::enable_if
#include <utility>     // std::move(), std::forward()

namespace ds {

/**
 * @brief A binary tree data structure.
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
 */
template <typename T, typename Algorithm = pre_order, typename Allocator = std::allocator<T>>
class tree {

    template <typename, typename, typename>
    friend class tree;

    template <typename, typename, bool>
    friend class tree_iterator;

    friend class binary_node<T>;

public:
    /*   ---   Types declarations   ---   */

    // General
    using value_type      = T;
    using reference       = value_type&;
    using const_reference = const value_type&;
    using node_type       = binary_node<T>;
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

    // Deleter functor
    struct delete_node {
        allocator_type allocator;
        constexpr delete_node() :
                allocator() {
        }
        delete_node(const delete_node&) = default;
        delete_node(allocator_type allocator) :
                allocator(allocator) {
        }
        void operator()(node_type* node) {
            assert(node != nullptr);
            // recursively delete children
            if (node->_left) {
                (*this)(node->_left);
            }
            if (node->_right) {
                (*this)(node->_right);
            }
            // destroy
            std::allocator_traits<allocator_type>::destroy(allocator, node);
            // deallocate
            std::allocator_traits<allocator_type>::deallocate(allocator, node, 1);
        }
    };

    // Allocator functor
    struct allocate_node {
        allocator_type allocator;
        constexpr allocate_node() :
                allocator() {
        }
        allocate_node(const allocate_node&) = default;
        allocate_node(allocator_type allocator) :
                allocator(allocator) {
        }

        template <typename... Args>
        std::unique_ptr<node_type, delete_node> operator()(Args&&... args) {
            using al = std::allocator_traits<allocator_type>;
            // allocate
            node_type* ptr = al::allocate(allocator, 1);
            // construct
            al::construct(allocator, ptr, std::forward<Args>(args)...);
            // return result
            return std::unique_ptr<node_type, delete_node>(ptr, {allocator});
        }
    };

protected:
    allocator_type allocator;
    /// @brief Owning pointer to the root node.
    std::unique_ptr<node_type, delete_node> _root;
    /// @brief The number of nodes in the tree.
    size_type _size;

public:
    /**
     * Create an empty tree.
     */
    constexpr tree() :
            allocator(),
            _root(),
            _size(0u) {
    }

    /**
     * Copy constructor. Create a tree by copying the tree passed as argument. Remember that this involves deep copy
     * (use std::move() whenever you can). A new {@link node_type} will be allocated for each node in the other tree.
     * The allocation will use the allocator.
     * @param other the tree to be copied from
     */
    tree(const tree& other) :
            _root(
                other._root
                    ? std::move(get_allocator()(*other._root, allocate_node(allocator)))
                    : decltype(_root){}),
            _size(other._size) {
    }

    /**
     * Move constructor. Create a tree by copying the tree passed as argument. Remember that this involves deep copy
     * (use std::move() whenever you can). A new {@link node_type} will be allocated for each node in the other tree.
     * The allocation will use the allocator.
     * @param other the tree to be copied from
     */
    tree(tree&& other) :
            _root(std::move(other._root)),
            _size(other._size) {
        other.nullify();
    }

    /**
     * Create a tree by copying the tree passed as argument. Remember that this involves deep copy (use std::move()
     * whenever you can). A new {@link node_type} will be allocated for each node in the other tree. The allocation will
     * use the allocator.
     * @param other the tree to be copied from
     */
    template <
        typename ConvertibleT,
        typename OtherAlg,
        typename OtherAlloc,
        CHECK_CONVERTIBLE_T>
    explicit tree(const tree<ConvertibleT, OtherAlg, OtherAlloc>& other) :
            _root(
                other._root
                    ? std::move(get_allocator()(*other._root, allocate_node(*this)))
                    : nullptr),
            _size(other._size) {
    }

    /**
     * Create a tree by moving the tree passed as argment. After this method, the tree passed as argument will be empty,
     * it's node will (logically) belong to this tree. This operation has constant time complexity.
     * @param other the tree to be moved
     */
    template <
        typename ConvertibleT,
        typename OtherAlg,
        typename OtherAlloc,
        CHECK_CONVERTIBLE_T>
    tree(tree<ConvertibleT, OtherAlg, OtherAlloc>&& other) :
            _root(std::move(other._root)),
            _size(other._size) {
    }

    /**
     * Create a tree by copying the nodes structure starting from the {@link temporary_node} passed as argument. A new
     * {@link node_type} will be allocated for each node in the structure passed. The allocation will use the allocator.
     * @param root the root of the newly created tree 
     */
    template <typename ConvertibleT, CHECK_CONVERTIBLE_T>
    tree(temporary_node<ConvertibleT>&& root) :
            _root(
                std::move(
                    get_allocator()(
                        root,
                        allocate_node(allocator)))),
            _size(root.get_size()) {
    }

    tree& operator=(const tree& other) {
        this->_root = std::move(get_allocator()(*other._root, allocate_node(allocator)));
        this->_size = other._size;
        return *this;
    }

    tree& operator=(tree&& other) {
        this->_root = other._root;
        this->_size = other._size;
        other.nullify();
        return *this;
    }

    ~tree() = default;

    /*   ---   Iterators   ---   */
    /**
     * @brief Returns an iterator to the beginning.
     * @details The iterator will point to the first element of the container. Which one is the first depends on the
     * algorithm used to traverse the tree.
     * @return iterator to the first element
     */
    template <typename A = Algorithm>
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
    template <typename A = Algorithm>
    const_iterator<A> begin() const {
        return cbegin();
    }

    /**
     * @copydoc #begin() const
     */
    template <typename A = Algorithm>
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
        return reverse_iterator<A>(iterator<A>(this));
    }

    template <typename A = Algorithm>
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

    template <typename A = Algorithm>
    const_reverse_iterator<A> crbegin() const {
        return const_reverse_iterator<A>(const_iterator<A>(this));
    }

    // reverse end
    template <typename A = Algorithm>
    reverse_iterator<A> rend() {
        // reverse_iterator takes an iterator as argument, we construct it using {this}
        return --reverse_iterator<A>(iterator<A>(this));
    }

    template <typename A = Algorithm>
    const_reverse_iterator<A> rend() const {
        return crend();
    }

    template <typename A = Algorithm>
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
        return _root == nullptr;
    }

    /**
     * @brief Returns the number of the nodes in this tree
     * @return the number of nodes
     */
    size_type size() const {
        return _size;
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
private:
    void nullify() {
        _root = nullptr;
        _size = 0;
    }

    template <typename It>
    iterator<typename It::algorithm_type>
    insert(It position, std::unique_ptr<node_type, delete_node> node) {
        node_type* target = position.get_node();
        if (target) { // if iterator points to valid node
            target->replace_with(*node);
        } else if (!this->_root) {
            this->_root = std::move(node);
        } else {
            throw std::logic_error("Tried to insert node in a not valid position.");
        }
        return iterator<typename It::algorithm_type>(this, node.get());
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
    template <typename OtherAlg, typename OtherAlloc>
    void swap(tree<T, OtherAlg, OtherAlloc>& other) {
        // I want unqualified name lookup in order to let invoking an user-defined swap function outside std namespace.
        using namespace std;
        swap(_root, other._root);
        swap(_size, other._size);
    }

    /**
     * Insert an element at the specified position by replacing the existent
     * node. This will use the copy semantics.
     * @param position where to insert the element
     * @param node an r-value reference to a temporary_node
     * @return an iterator that points to the inserted element
     */
    template <typename It>
    iterator<typename It::algorithm_type> insert(It position, temporary_node<T>&& node) {
        this->_size += node.get_size() - 666; // TODO perform calculation
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
        ++this->_size;
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
        ++this->_size;
        return insert(position, get_allocator()(std::move(value)));
    }

    template <typename It, typename... Args>
    iterator<typename It::algorithm_type> emplace(It position, Args&&... args) {
        ++this->_size;
        return insert(position, get_allocator()(std::forward<Args>(args)...));
    }

    /*   ---   Equality comparison   ---   */
    template <
        typename ConvertibleT = T,
        typename OtherAlg,
        typename OtherAlloc,
        CHECK_CONVERTIBLE_T>
    bool operator==(const tree<T, OtherAlg, OtherAlloc>& other) const {
        // 1. Test if different size
        if (this->_size != other._size) {
            return false;
        }
        // 2. Test if one between this or other has a root that is set while the other doesn't.
        if ((this->_root == nullptr) != (other._root == nullptr)) {
            return false;
        }
        // At the end is either null (both) or same as the other.
        return this->_root == nullptr || *this->_root == *other._root;
    }

    template <
        typename ConvertibleT = T,
        typename OtherAlg,
        typename OtherAlloc,
        CHECK_CONVERTIBLE_T>
    bool operator!=(const tree<ConvertibleT, OtherAlg, OtherAlloc>& other) const {
        return !(*this == other);
    }

    template <typename ConvertibleT, CHECK_CONVERTIBLE_T>
    bool operator==(const temporary_node<ConvertibleT>& other) const {
        // Test if different size (trivial case for performance)
        if (this->_size != other.get_size()) {
            return false;
        }
        // Deep test for equality
        return _root && *_root == other;
    }

    template <typename ConvertibleT, CHECK_CONVERTIBLE_T>
    bool operator!=(const temporary_node<ConvertibleT>& other) const {
        return !(*this == other);
    }

public:
    /*   ---   Memory management functors   ---   */
    allocate_node get_allocator() {
        return {allocator};
    }

    delete_node get_deleter() {
        return {allocator};
    };
};

} // namespace ds
