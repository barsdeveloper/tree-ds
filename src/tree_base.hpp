#ifndef TREE_BASE_H_
#define TREE_BASE_H_

#include <cstddef> // std::size_t, std::ptrdiff_t

namespace bds {

template <typename> class node;
template <typename, typename, bool> class tree_iterator;

/*
 * tree_base<T>
 * The purpose of this class is to have a type that depends only on the value_type template parameter. As iterators need
 * to keep a pointer to the tree<T> class (to obtain the root node), all the iterators of this container can depend only
 * on value_type (scary iterator) and, eventually other types not related to the container. Doing so you can assign
 * iterators of different container type (but with the same value_type, e.g.: different allocator) to each other.
 * REMEMBER: this is not required by the standard: you shouldn't rely on this behavior for STL containers.
 */

template <typename T>
class tree_base {

	template <typename, typename, bool> friend class tree_iterator;

public:
	using size_type = std::size_t;

protected:
	node<T>* _root;
	size_type _size;
	tree_base(node<T> *root, size_type size);

public:
	/*
	 * This class is not CopyConstructible nor CopyAssignable because allowing the copy involves the allocation of
	 * memory for the object pointed by _root attribute. This is impossible without using the allocator which type is
	 * known only in the subclass tree<T>. Why this design choice? I want iterators to not depend on the allocator used
	 * by the container (SCARY iterator). Iterators need to keep a pointer to the tree, so they can point to
	 * tree_base<T> not to tree<T, Allocator> and consequently depend only on the parameter T, not both T and Allocator.
	 * The copy semantics is implemented in the sublacc tree<T>.
	 */
	tree_base();                                      // Default constructor
	tree_base(const tree_base&) = delete;             // Copy constructor
	tree_base& operator =(const tree_base&) = delete; // Copy assignment operator
	tree_base(tree_base&&);                           // Move constructor
	tree_base& operator =(tree_base&&);               // Move assignment operator
	virtual ~tree_base();                             // Destructor

};

} /* namespace bds */

#include "tree_base.tpp"

#endif /* TREE_BASE_HPP_ */
