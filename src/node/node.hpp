#ifndef NODE_HPP_
#define NODE_HPP_

namespace ds {

template <typename, typename, typename> class tree;

template <typename T>
class node {

	template <typename, typename, typename> friend class tree;
	template <typename, typename, bool> friend class tree_iterator;
	friend class pre_order;

protected:
	T _argument;
	node *_parent;
	node *_prev_sibling;
	node *_next_sibling;
	node *_first_child;
	node *_last_child;

	node(const node&) = default;
	node(node&&);

	node(const T &argument);
	node(T &&argument);
	template <typename ...Args> node(Args&& ...args);

	void release_node();
	void insert(node&);
	void append_child(node&);
	void prepend_child(node&);

public:
	~node();

};

} /* namespace ds */

#include "node.tpp"

#endif /* NODE_HPP_ */
