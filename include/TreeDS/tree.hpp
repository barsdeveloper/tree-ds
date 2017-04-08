#ifndef H454503B6_5C98_4082_8BCC_12BDF76731AE
#define H454503B6_5C98_4082_8BCC_12BDF76731AE

#include <exception> // std::exception
#include <limits>    // std::numeric_limits()
#include <memory>    // std::unique_ptr, std::make_unique
#include <utility>   // std::move()

#include <TreeDS/tree_base.hpp>
#include <TreeDS/node.hpp>
#include <TreeDS/temporary_node.hpp>
#include <TreeDS/tree_iterator.hpp>
#include <TreeDS/iterator/pre_order.hpp>
#include <TreeDS/iterator/in_order.hpp>
#include <TreeDS/iterator/post_order.hpp>

namespace ds {

template<typename T, typename Algorithm = pre_order,
		typename Allocator = std::allocator<T>>
class tree: public tree_base<T> {

public:
	/*   ---   Types declarations   ---   */
	using typename tree_base<T>::value_type;
	using typename tree_base<T>::node_type;
	using typename tree_base<T>::reference;
	using typename tree_base<T>::const_reference;
	using typename tree_base<T>::size_type;
	using typename tree_base<T>::difference_type;
	using pointer = typename std::allocator_traits<Allocator>::pointer;
	using const_pointer = typename std::allocator_traits<Allocator>::const_pointer;
	typedef Algorithm algorithm_type;
	using allocator_type = Allocator;

	// Iterator types
	template<typename A>
	using iterator = tree_iterator<T, A, false>;

	template<typename A>
	using const_iterator = tree_iterator<T, A, true>;

	template<typename A>
	using reverse_iterator = std::reverse_iterator<iterator<A>>;

	template<typename A>
	using const_reverse_iterator = std::reverse_iterator<const_iterator<A>>;

public:
	tree() = default;

	// REMEMBER: this involves deep copy. Use std::move() whenever you can
	tree(const tree& other) :
			tree_base<T>(other._root ?
			// TODO use allocators
					std::make_unique<node_type>(*other._root) : nullptr,
					other._size) {
	}

	tree(tree&&) = default;

	tree(temporary_node<T> && root) :
			// TODO use allocators
			tree_base<T>(std::make_unique<node_type>(std::move(root)),
					root.get_size()) {
	}

	tree& operator =(const tree& other) {
		// TODO use allocators
		this->_root = std::make_unique<node_type>(*other.root);
		this->_size = other.size;
		return *this;
	}

	tree& operator =(tree&&) = default;

	~tree() = default;

	static temporary_node<T> produce_node(T value) {
		return {value};
	}

	/*   ---   Iterators   ---   *
	 * r => reverse
	 * c => constant
	 */
	// begin
	auto begin() {
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
		return ++iterator<Algorithm>(*this);
	}

	auto begin() const {
		return cbegin();
	}

	auto cbegin() const {
		// Confused by the incrementation (++)? See the comment above.
		return ++const_iterator<Algorithm>(*this);
	}

	// end
	auto end() {
		return iterator<Algorithm>(*this);
	}

	auto end() const {
		return cend();
	}

	auto cend() const {
		return const_iterator<Algorithm>(*this);
	}

	// reverse begin
	auto rbegin() {
		return reverse_iterator<Algorithm>(*this);
	}

	auto rbegin() const {
		return crbegin();
	}

	auto crbegin() const {
		return const_reverse_iterator<Algorithm>(*this);
	}

	// reverse end
	auto rend() {
		return --reverse_iterator<Algorithm>(*this);
	}

	auto rend() const {
		return crend();
	}

	auto crend() const {
		return --const_reverse_iterator<Algorithm>(*this);
	}

	/*   ---   Modifiers   ---   */
private:
	template<typename It>
	iterator<typename It::algorithm_type> insert(It position,
			std::unique_ptr<node_type> n) {
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
	template<typename It>
	iterator<typename It::algorithm_type> insert(It position,
			temporary_node<T> && node) {
		return insert(position, std::make_unique<node_type>(node));
	}

	template<typename It>
	iterator<typename It::algorithm_type> insert(It position,
			const_reference value) {
		return insert(position, std::make_unique<node_type>(value));
	}

	template<typename It>
	iterator<typename It::algorithm_type> insert(It position,
			value_type&& value) {
		return insert(position, std::make_unique<node_type>(std::move(value)));
	}

	template<typename It, typename ...Args>
	iterator<typename It::algorithm_type> emplace(It position, Args&&... args) {
		return insert(position, std::make_unique<node_type>(args...));
	}

};

} /* namespace ds */

#endif /* H454503B6_5C98_4082_8BCC_12BDF76731AE */
