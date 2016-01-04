#ifndef NODE_HPP_
#define NODE_HPP_

namespace ds {

template <typename > class tree_base;
template <typename, typename, typename > class tree;

template <typename T>
class node {

	template <typename, typename, typename > friend class tree;
	template <typename, typename, bool> friend class tree_iterator;
	friend class pre_order;

protected:
	T _argument;
	node *_parent;
	node *_prev_sibling;
	node *_next_sibling;
	node *_first_child;
	node *_last_child;

	node(const node&) = delete;
	node(node&&);
	node(const T &argument);
	node(T &&argument);
	template <typename ...Args> node(Args&& ...args);
	node(tree_base<T> &&);

	void unlink();
	void insert(node&);
	void append_child(node&);
	void prepend_child(node&);
	void append_sibling(node&);
	// void prepend_sibling(node&); // not needed

public:
	~node();

	// Parameters for these methods are passed by copy to allow both l-values and r-values and not modify their trees
	node& operator ()(const node&);
	node& operator ,(const node&);

};

} /* namespace ds */

#include "../../include/TreeDS/node/node.tpp"

#endif /* NODE_HPP_ */
