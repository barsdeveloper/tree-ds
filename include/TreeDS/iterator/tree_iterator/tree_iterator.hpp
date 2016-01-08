#ifndef H3B68D6D4_220A_4EFE_8556_D06377C8C61C
#define H3B68D6D4_220A_4EFE_8556_D06377C8C61C

#include <iterator>    // std::iterator
#include <type_traits> // std::conditional, std::enable_if

namespace ds {

template <typename> class tree_base;
template <typename, typename, typename> class tree;
template <typename> class node;

template <typename T, typename Algorithm, bool Is_Const = false>
class tree_iterator: public std::iterator<std::bidirectional_iterator_tag, T> {

	friend class tree_iterator<T, Algorithm, !Is_Const> ; // const and non const are each other friends
	template <typename, typename, typename> friend class tree;

public:
	/*
	 * REMEMBER:
	 * There is no conceptual difference between the container and data it holds. If the container is constant, both
	 * the container and data should be constant.
	 */
	using value_type = typename std::conditional<Is_Const, const T, T>::type;
	using node_type = typename std::conditional<Is_Const, const node<T>, node<T>>::type;
	using tree_type = typename std::conditional<Is_Const, const tree_base<T>, tree_base<T>>::type;
	using algorithm_type = const Algorithm;

protected:
	algorithm_type &_algorithm;
	tree_type *_tree; // nullptr => no container associated (default iterator)
	node_type *_node; // nullptr => end()

public:
	constexpr tree_iterator();                       // Default constructor
	tree_iterator(const tree_iterator&);             // Copy constructor
	tree_iterator& operator =(const tree_iterator&); // Copy assignment operator
	~tree_iterator() = default;                      // Destructor
	tree_iterator(tree_type &tree);
	tree_iterator(tree_type &tree, node_type &current);
	/*
	 * Allow iterator to const_iterator conversion (parameter U is just to have a dependent parameter and delay
	 * enable_if evaluation until template specialization)
	 */
	template <typename U = T, typename = typename std::enable_if<Is_Const && std::is_same<U, T>::value>::type>
	tree_iterator(const tree_iterator<T, Algorithm, false>&);
	node_type* node() const;
	bool is_end() const;
	value_type& operator *() const;
	value_type* operator ->() const;
	bool operator ==(const tree_iterator&) const;
	bool operator !=(const tree_iterator&) const;
	tree_iterator& operator ++();
	tree_iterator operator ++(int);
	tree_iterator& operator --();
	tree_iterator operator --(int);

};

} /* namespace ds */

#include "tree_iterator.tpp"

#endif /* H3B68D6D4_220A_4EFE_8556_D06377C8C61C */
