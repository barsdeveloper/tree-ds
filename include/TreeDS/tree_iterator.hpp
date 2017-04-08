#ifndef H3B68D6D4_220A_4EFE_8556_D06377C8C61C
#define H3B68D6D4_220A_4EFE_8556_D06377C8C61C

#include <iterator>    // std::iterator
#include <type_traits> // std::conditional, std::enable_if

#include <TreeDS/node.hpp>

namespace ds {

template<typename > class tree_base;
template<typename, typename, typename > class tree;
template<typename > class node;

template<typename T, typename Algorithm, bool Is_const = false>
class tree_iterator: public std::iterator<std::bidirectional_iterator_tag, T> {

	// const and non const are each other friends
	friend class tree_iterator<T, Algorithm, !Is_const> ;
	template<typename, typename, typename >
	friend class tree;

public:
	using value_type = typename std::conditional<Is_const, const T, T>::type;
	using node_type = typename std::conditional<Is_const, const node<T>, node<T>>::type;
	using tree_type = typename std::conditional<Is_const, const tree_base<T>, tree_base<T>>::type;
	using algorithm_type = const Algorithm;

protected:
	algorithm_type algorithm;
	tree_type *tree; // nullptr => no container associated (default iterator)
	node_type *current; // nullptr => end()

	tree_iterator(tree_type* tree, node_type* current) :
			tree(tree), current(current) {
	}

public:
	constexpr tree_iterator() :
			algorithm(), tree(nullptr), current(nullptr) {
	}

	tree_iterator(const tree_iterator&) = default;

	tree_iterator& operator =(const tree_iterator& other) {
		tree = other.tree;
		current = other.current;
		return *this;
	}

	~tree_iterator() = default;

	tree_iterator(tree_type& tree) :
			algorithm(), tree(&tree), current(nullptr) {
	}

	/*
	 * Allow iterator to const_iterator conversion (parameter U is just to have
	 * a dependent parameter and delay enable_if evaluation until template
	 * specialization)
	 */
	template<typename U = T, typename = typename std::enable_if<
			Is_const && std::is_same<U, T>::value>::type>
	tree_iterator(const tree_iterator<T, Algorithm, false>& iterator) :
			algorithm(Algorithm { }), tree(iterator.tree), current(
					iterator.current) {
	}

	node_type* get_node() const {
		return current;
	}

	value_type& operator *() const {
		return current->_value;
	}

	value_type* operator ->() const {
		return &(current->_value);
	}

	bool operator ==(const tree_iterator& other) const {
		return tree == other.tree && current == other.current;
	}

	bool operator !=(const tree_iterator& other) const {
		return !(*this == other);
	}

	tree_iterator& operator ++() {
		if (current) {
			// const_cast needed in case node_type is non const
			current = const_cast<node_type*>(algorithm.increment(*current));
		} else if (tree && tree->_root) {
			/*
			 * If iterator is at the end():
			 *     normal iterator  => incremented from end() => go to its first element (rewind)
			 *     reverse iterator	=> decremented from end() => go to its last element (before end())
			 * REMEMBER: ++ operator on a reverse_iterator delegates to -- operator of tree_iterator and vice versa
			 */
			// const_cast needed in case node_type is non const
			current = const_cast<node_type*>(algorithm.go_first(*tree->_root));
		}
		return *this;
	}

	tree_iterator operator ++(int) {
		tree_iterator it(*this);
		++(*this);
		return it;
	}

	tree_iterator& operator --() {
		if (current) {
			node_type *temp = current;
			// const_cast needed in case node_type is non const
			current = const_cast<node_type*>(algorithm.decrement(*current));
		} else if (tree && tree->_root) {
			/*
			 * If iterator is at the end():
			 *     normal iterator  => decremented from end() => go to its last element (before end())
			 *     reverse iterator => incremented from end() => go to its first element (rewind)
			 * REMEMBER: ++ operator on a reverse_iterator delegates to -- operator of tree_iterator and vice versa
			 */
			// const_cast needed in case node_type is non const
			current = const_cast<node_type*>(algorithm.go_last(*tree->_root));
		}
		return *this;
	}

	tree_iterator operator --(int) {
		tree_iterator it(*this);
		--(*this);
		return it;
	}

};

} /* namespace ds */

#endif /* H3B68D6D4_220A_4EFE_8556_D06377C8C61C */
