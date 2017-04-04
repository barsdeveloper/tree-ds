#ifndef H2FA9DD71_C0FC_409B_82F9_A6BCC3512038
#define H2FA9DD71_C0FC_409B_82F9_A6BCC3512038

#include <memory> // std::unique_ptr, std::move()

namespace ds {

template<typename T>
class node {

	template<typename, typename, bool>
	friend class tree_iterator;

	template<typename, typename, typename >
	friend class tree;

protected:
	T _value;
	node<T>* _parent;
	std::unique_ptr<node<T>> _left;
	std::unique_ptr<node<T>> _right;

	void set_left(std::unique_ptr<node<T>> n) {
		_left = std::move(n);
		if (_left)
			_left->_parent = this;
	}

	void set_right(std::unique_ptr<node<T>> n) {
		_right = std::move(n);
		if (_right)
			_right->_parent = this;
	}

public:
	node(T value) :
			_value(value), _parent(nullptr), _left(nullptr), _right(nullptr) {
	}

	/*template <typename ...Args>
	 node(Args&& ...args) :
	 value(args...), _parent(nullptr), _left(nullptr), _right(nullptr) {
	 }*/

	node(const node& other) :
			_value(other._value), _parent(nullptr) {
		if (other._left) {
			// TODO use allocators
			this->set_left(std::make_unique<node<T>>(*other._left));
		}
		if (other._right) {
			// TODO use allocators
			this->set_right(std::make_unique<node<T>>(*other._right));
		}
	}

	node(node&& other) :
			_value(other._value), _parent(other._parent), _left(
					std::move(other._left)), _right(std::move(other._right)) {
		if (_left)
			_left->_parent = this;
		if (_right)
			_right->_parent = this;
		other._parent = nullptr;
	}

	~node() = default;

	/*   ---   Getters   ---   */
	const node* parent() const {
		return _parent;
	}

	const node* left_child() const {
		return _left.get();
	}

	const node* right_child() const {
		return _right.get();
	}

	const node* first_child() const {
		// will be null if no children
		return _left ? _left.get() : _right.get();
	}

	const node* last_child() const {
		// will be null if no children
		return _right ? _right.get() : _left.get();
	}

	long hash_code() const;

	bool operator ==(const node& other) {
		/*
		 * Case when one between this or other has a left or right that is set
		 * while the other doesn't.
		 */
		if ((this->_left == nullptr) != (other._left == nullptr)
				|| (this->_right == nullptr) != (other._right == nullptr)) {
			return false;
		}
		// Test hold value for inequality
		if (this->_value != other._value) {
			return false;
		}
		if (this->_left) {
			if (*this->_left != *other._left) {
				return false;
			}
		}
		if (this->_right) {
			if (*this->_right != *other._right) {
				return false;
			}
		}
		// all the reasons to return false were discarded
		return true;
	}

	bool operator!=(const node& other) {
		return !(*this == other);
	}

	/*   ---   Tree construction   ---   */
	void insert(std::unique_ptr<node<T>> n) {
		n->_parent = this;
		if (!_left) {
			_left = std::move(n);
		} else if (!_right) {
			_right = std::move(n);
		} else {
			throw std::logic_error("This node already has 2 children.");
		}
	}

};

} /* namespace ds */

#endif /* H2FA9DD71_C0FC_409B_82F9_A6BCC3512038 */
