#ifndef SRC_TREEDS_ITERATOR_IN_ORDER_HPP_
#define SRC_TREEDS_ITERATOR_IN_ORDER_HPP_

namespace ds {

template<typename T>
class node;

class pre_order final {

public:
	constexpr pre_order() = default;
	~pre_order() = default;

	template<typename T>
	const node<T>* increment(const node<T>& n) const {
		const node<T>* prev = &n;
		const node<T>* next = n.parent();
		if (prev == next->left_child()) {
			return next;
		}
		if (next->left_child()) {
			if (next->right_child()) {
				next = next->right_child();
				while(next->first_child()) {
					next = next->first_child();
				}
			} else {
				next = next->parent();
			}
		} else {

		}
	}

	template<typename T>
	const node<T>* decrement(const node<T>& n) const {
		const node<T>* next = n.parent();
		const node<T>* prev = &n;
		/*
		 * The parent is the next node if (REMEMBER: we traverse tree in pre-order and decrement the iterator):
		 *     1) The passed node is root (its parent is nullptr so the previous value is the end of the reverse iterator).
		 *     2) The node is the unique child of its parent
		 */
		if (!next || next->first_child() == prev) {
			return next;
		} else {
			/*
			 * Here we are in the situation where the passed node (n) is a right child of a parent that has also a left
			 * child. All we have to do is find the lowest and rightmost node in the tree structure starting from the left
			 * sibling.
			 */
			next = next->left_child();
			while (next->last_child()) {
				next = next->last_child();
			}
		}
		return next;
	}

	template<typename T> const node<T>* go_first(const node<T>& root) const {
		const node<T>* result = &root;
		while(result->left_child()) {
			result = result->left_child();
		}
		return result;
	}

	template<typename T>
	const node<T>* go_last(const node<T>& root) const {
		const node<T>* result = &root;
		while(result->right_child()) {
			result = result->right_child();
		}
		return result;
	}

};

} /* namespace ds */

#endif /* SRC_TREEDS_ITERATOR_IN_ORDER_HPP_ */
