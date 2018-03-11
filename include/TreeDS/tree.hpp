#pragma once

#include <TreeDS/binary_node.hpp>
#include <TreeDS/iterator/in_order.hpp>
#include <TreeDS/iterator/post_order.hpp>
#include <TreeDS/iterator/pre_order.hpp>
#include <TreeDS/std_implementations.hpp>
#include <TreeDS/temporary_node.hpp>
#include <TreeDS/tree_base.hpp>
#include <TreeDS/tree_iterator.hpp>
#include <functional>  // std::bind(), std::mem_fun()
#include <limits>      // std::numeric_limits()
#include <memory>      // std::unique_ptr, std::allocator_traits
#include <stdexcept>   // std::logic_error
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
class tree : public tree_base<T> {

    template <typename, typename, typename>
    friend class tree;

    friend class binary_node<T>;

public:
    /*   ---   Types declarations   ---   */

    // General
    using typename tree_base<T>::value_type;
    using typename tree_base<T>::node_type;
    using typename tree_base<T>::reference;
    using typename tree_base<T>::const_reference;
    using typename tree_base<T>::size_type;
    using typename tree_base<T>::difference_type;
    using pointer        = typename std::allocator_traits<Allocator>::pointer;
    using const_pointer  = typename std::allocator_traits<Allocator>::const_pointer;
    using algorithm_type = Algorithm;
    using allocator_type = typename std::allocator_traits<Allocator>::template rebind_alloc<node_type>;

    // Iterators
    template <typename A>
    using iterator = tree_iterator<T, A, false>;

    template <typename A>
    using const_iterator = tree_iterator<T, A, true>;

    template <typename A>
    using reverse_iterator = std::reverse_iterator<iterator<A>>;

    template <typename A>
    using const_reverse_iterator = std::reverse_iterator<const_iterator<A>>;

    // Deleter functor
    class node_deleter {
        std::reference_wrapper<tree> t;

    public:
        node_deleter(const node_deleter&) = default;
        node_deleter(tree& t) :
                t(t) {
        }
        void operator()(node_type* node) const {
            assert(node != nullptr);
            // recursively delete children
            if (node->_left) {
                (*this)(node->_left);
            }
            if (node->_right) {
                (*this)(node->_right);
            }
            // destroy
            std::allocator_traits<allocator_type>::destroy(t.get().allocator, node);
            // deallocate
            std::allocator_traits<allocator_type>::deallocate(t.get().allocator, node, 1);
        }
    };

    // Allocator functor
    class node_allocator {
        std::reference_wrapper<tree> _tree;

    public:
        node_allocator(const node_allocator&) = default;
        node_allocator(tree& tree) :
                _tree(tree) {
        }

        template <typename... Args>
        std::unique_ptr<node_type, node_deleter> operator()(Args&&... args) const {
            using al = std::allocator_traits<allocator_type>;
            // allocate
            node_type* ptr = al::allocate(_tree.get().allocator, 1);
            // construct
            al::construct(_tree.get().allocator, ptr, std::forward<Args>(args)...);
            // return result
            return std::unique_ptr<node_type, node_deleter>(ptr, _tree.get().get_deleter());
        }
    };

protected:
    allocator_type allocator;

public:
    /**
	 * Create an empty tree.
	 */
    constexpr tree() = default;

    /**
     * Copy constructor. Create a tree by copying the tree passed as argument. Remember that this involves deep copy
     * (use std::move() whenever you can). A new {@link node_type} will be allocated for each node in the other tree.
     * The allocation will use the allocator.
     * @param other the tree to be copied from
     */
    tree(const tree& other) :
            tree_base<T>(
                other._root
                    ? get_allocator()(*other._root, node_allocator(*this)).release()
                    : nullptr,
                other._size) {
    }

    /**
     * Move constructor. Create a tree by copying the tree passed as argument. Remember that this involves deep copy
     * (use std::move() whenever you can). A new {@link node_type} will be allocated for each node in the other tree.
     * The allocation will use the allocator.
     * @param other the tree to be copied from
     */
    tree(tree&& other) :
            tree_base<T>(other._root, other._size) {
        other.nullify();
    }

    /**
     * Create a tree by copying the tree passed as argument. Remember that this involves deep copy (use std::move()
     * whenever you can). A new {@link node_type} will be allocated for each node in the other tree. The allocation will
     * use the allocator.
     * @param other the tree to be copied from
     */
    template <
        typename OtherT,
        typename OtherAlg,
        typename OtherAlloc,
        typename = std::enable_if_t<std::is_convertible_v<OtherT, T>>>
    explicit tree(const tree<OtherT, OtherAlg, OtherAlloc>& other) :
            tree_base<T>(
                other._root
                    ? get_allocator()(*other._root, node_allocator(*this)).release()
                    : nullptr,
                other._size) {
    }

    /**
     * Create a tree by moving the tree passed as argment. After this method, the tree passed as argument will be empty,
     * it's node will (logically) belong to this tree. This operation has constant time complexity.
     * @param other the tree to be moved
     */
    template <typename OtherAlg, typename OtherAlloc>
    tree(tree<T, OtherAlg, OtherAlloc>&& other) :
            tree_base<T>(other._root, other._size) {
    }

    /**
     * Create a tree by copying the nodes structure starting from the {@link temporary_node} passed as argument. A new
     * {@link node_type} will be allocated for each node in the structure passed. The allocation will use the allocator.
     * @param root the root of the newly created tree 
     */
    template <
        typename ConvertibleT,
        typename = std::enable_if_t<
            std::is_convertible_v<ConvertibleT, T>>>
    tree(temporary_node<ConvertibleT>&& root) :
            tree_base<T>(
                get_allocator()(std::move(root), node_allocator(*this)).release(),
                root.get_size()) {
    }

    tree& operator=(const tree& other) {
        this->_root = get_allocator()(*other.root, node_allocator(*this)).release();
        this->_size = other._size;
        return *this;
    }

    tree& operator=(tree&& other) {
        this->_root = other._root;
        this->_size = other._size;
        other.nullify();
        return *this;
    }

    ~tree() {
        if (this->_root != nullptr) {
            get_deleter()(this->_root);
        }
        this->nullify();
    }

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
	 * @details The iterator will point to the first element of the container. Which one is the first depends on the algorithm
	 * used to traverse the tree. Using this iterator you can't modify the container.
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
	 * The iterator will point one step after the last element of the container. Which one is the last depends on the algorithm used to traverse the tree. It's perfectly legitimate to decrement this iterator to obtain the last
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

    /**
	 * @brief Removes all elements from the tree.
	 * @details Invalidates any references, pointers, or iterators referring to contained elements. Any past-the-last
	 * element iterator ({@link tree#end() end()}) remains valid.
	 */
    void clear() {
        get_deleter()(this->_root);
        this->nullify();
    }

    /*   ---   Modifiers   ---   */
private:
    template <typename It>
    iterator<typename It::algorithm_type>
    insert(It position, std::unique_ptr<node_type, std::function<void(node_type*)>> n) {
        node_type* target_node = position.get_node();
        node_type* node        = n.release();
        if (target_node) { // if iterator points to valid node
            target_node->replace_with(*node);
        } else if (!this->_root) {
            this->_root = node;
        } else {
            throw std::logic_error("Tried to insert node in a not valid position.");
        }
        return iterator<typename It::algorithm_type>(this, node);
    }

public:
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

public:
    /*   ---   Memory management functors   ---   */
    node_allocator get_allocator() {
        return {*this};
    }

    node_deleter get_deleter() {
        return {*this};
    };
};

} // namespace ds
