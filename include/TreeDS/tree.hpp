#ifndef H454503B6_5C98_4082_8BCC_12BDF76731AE
#define H454503B6_5C98_4082_8BCC_12BDF76731AE

#include <TreeDS/iterator/in_order.hpp>
#include <TreeDS/iterator/post_order.hpp>
#include <TreeDS/iterator/pre_order.hpp>
#include <TreeDS/node.hpp>
#include <TreeDS/std_implementations.hpp>
#include <TreeDS/temporary_node.hpp>
#include <TreeDS/tree_base.hpp>
#include <TreeDS/tree_iterator.hpp>
#include <exception>   // std::exception
#include <limits>      // std::numeric_limits()
#include <memory>      // std::unique_ptr, std::make_unique
#include <type_traits> // std::enable_if
#include <utility>     // std::move()

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

public:
    /*   ---   Types declarations   ---   */
    using typename tree_base<T>::value_type;
    using typename tree_base<T>::node_type;
    using typename tree_base<T>::reference;
    using typename tree_base<T>::const_reference;
    using typename tree_base<T>::size_type;
    using typename tree_base<T>::difference_type;
    using pointer        = typename std::allocator_traits<Allocator>::pointer;
    using const_pointer  = typename std::allocator_traits<Allocator>::const_pointer;
    using algorithm_type = Algorithm;
    using allocator_type = Allocator;
    /**
	 * A bi-directional iterator used to traverse the tree.
	 * @tparam A traversal algorithm used by the iterator
	 */
    template <typename A>
    using iterator = tree_iterator<T, A, false>;

    /**
	 * A bi-directional constant iterator used to traverse the tree. You can't modify the tree using this iterator.
	 * @tparam A traversal algorithm used by the iterator
	 */
    template <typename A>
    using const_iterator = tree_iterator<T, A, true>;

    /**
	 * A bi-directional reverse iterator used to traverse the tree.
	 * @tparam A traversal algorithm used by the iterator
	 */
    template <typename A>
    using reverse_iterator = std::reverse_iterator<iterator<A>>;

    /**
	 * A bi-directional reverse iterator used to traverse the tree. You can't modify the tree using this iterator.
	 * @tparam A traversal algorithm used by the iterator
	 */
    template <typename A>
    using const_reverse_iterator = std::reverse_iterator<const_iterator<A>>;

public:
    /**
	 * Create an empty tree.
	 */
    constexpr tree() = default;

    // REMEMBER: this involves deep copy. Use std::move() whenever you can
    tree(const tree& other) :
        tree_base<T>(
            other._root ? std::make_unique<node_type>(*other._root) : nullptr,
            other._size) {
    }

    tree(tree&&) = default;

    template <typename OtherAlg, typename OtherAlloc>
    tree(const tree<T, OtherAlg, OtherAlloc>& other) :
        tree_base<T>(
            other._root ? std::make_unique<node_type>(*other._root) : nullptr,
            other._size) {
    }

    template <typename OtherAlg, typename OtherAlloc>
    tree(tree<T, OtherAlg, OtherAlloc>&& other) :
        tree_base<T>(std::move(other._root), other._size) {
    }

    tree(temporary_node<T>&& root) :
        // TODO use allocators
        tree_base<T>(
            std::make_unique<node_type>(std::move(root)),
            root.get_size()) {
    }

    tree& operator=(const tree& other) {
        // TODO use allocators
        this->_root = std::make_unique<node_type>(*other.root);
        this->_size = other.size;
        return *this;
    }

    tree& operator=(tree&&) = default;

    ~tree() = default;

    static temporary_node<T> produce_node(T value) {
        return {value};
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
        /*
		 * Why the incrementation (++)?
		 * Normally a tree_iterator<T> is constructed by taking a reference to a
		 * tree<T> object. The iterator constructed this way points to the
		 * end(). This end() status is the same for both iterator and
		 * reverse_iterator. Incrementing an iterator at the end() will take it
		 * to the first element (this is why we (++) here). Decrementing an
		 * iterator at the end() will take it to the last element (end is one
		 * step past last). std::reverse_iterator will take an underlying
		 * iterator as argument and translate a (++) operator on itself in a
		 * (--) operation on the underlying iterator and vice versa.
		 *
		 * Why we don't increment (++) the reverse iterator as we do with
		 * iterator? It would make sense: when constructed the iterator is at
		 * the end() (which is same for both iterator and reverse_iterator)
		 * incrementing will result in an actual decrementing taking the
		 * iterator to the last element (that is reverse_iterator's fist
		 * element). We don't need to do this because a reverse_iterator will
		 * be decremented before dereferenced (every time).
		 * See "http://en.cppreference.com/w/cpp/iterator/reverse_iterator/operator*".
		 * This seems like a waste of resources to me but this is how it works.
		 *
		 * Also the reason reverse end iterator are incremented should be clear
		 * now. In general reverse_iterator == iterator - 1
		 */
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
        // Confused by the incrementation (++)? See the comment above.
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

    // reverse begin
    template <typename A = Algorithm>
    reverse_iterator<A> rbegin() {
        return reverse_iterator<A>(iterator<A>(this));
    }

    template <typename A = Algorithm>
    const_reverse_iterator<A> rbegin() const {
        return crbegin();
    }

    template <typename A = Algorithm>
    const_reverse_iterator<A> crbegin() const {
        return const_reverse_iterator<A>(const_iterator<A>(this));
    }

    // reverse end
    template <typename A = Algorithm>
    reverse_iterator<A> rend() {
        // reverse_iterator takes an iterator as argument, we construcit using {this}
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

    /*   ---   Modifiers   ---   */
private:
    template <typename It>
    iterator<typename It::algorithm_type> insert(It position, std::unique_ptr<node_type> n) {
        auto p = position.get_node(); // position
        if (p) {
            p->substitute_with(std::move(n));
        } else if (!this->_root) {
            this->_root = std::move(n);
        } else {
            throw std::logic_error(
                "Tried to insert node in a not valid position.");
        }
        ++this->_size;
        return iterator<typename It::algorithm_type>(this, n.get());
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
        return insert(position, std::make_unique<node_type>(value));
    }

    /**
	 * Insert an element at the specified position by replacing the existent
	 * node. This will use the move semantics.
	 * @param position where to insert the element
	 * @param value to insert in the tree, that will be copied
	 * @return an iterator that points to the inserted element
	 */
    template <typename It>
    iterator<typename It::algorithm_type> insert(It position, value_type&& value) {
        return insert(position, std::make_unique<node_type>(std::move(value)));
    }

    template <typename It, typename... Args>
    iterator<typename It::algorithm_type> emplace(It position, Args&&... args) {
        return insert(position, std::make_unique<node_type>(args...));
    }
};

} /* namespace ds */

#endif /* H454503B6_5C98_4082_8BCC_12BDF76731AE */
