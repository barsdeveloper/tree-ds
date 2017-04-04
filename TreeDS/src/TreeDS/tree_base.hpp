#ifndef H89E2543D_D005_4E9B_8B4A_C0BDF8AEC5A8
#define H89E2543D_D005_4E9B_8B4A_C0BDF8AEC5A8

#include <cstddef> // std::size_t
#include <memory>  // std::unique_ptr
#include <TreeDS/node.hpp>

namespace ds {

template<typename, typename, bool> class tree_iterator;

/*
 * The purpose of this class is to have a type that depends only on the value_type template parameter. As iterators need
 * to keep a pointer to the Tree<T> class (to obtain the root node), all the iterators of this container can depend only
 * on value_type (scary iterator) and, eventually other types not related to the container. Doing so you can assign
 * iterators of different container type (but with the same value_type, e.g.: different allocator) to each other.
 * REMEMBER: this is not required by the standard: you shouldn't rely on this behavior for STL containers.
 */
template<typename T>
class tree_base {

	template<typename, typename, bool> friend class tree_iterator;

public:
	using value_type = T;
	using reference = value_type&;
	using const_reference = const reference;
	using node_type = node<T>;
	using size_type = std::size_t;
	using difference_type = std::ptrdiff_t;

protected:
	std::unique_ptr<node_type> _root;
	size_type _size;

protected:
	tree_base() :
			_root(nullptr), _size(0) {
	}

	// Copy logic is implemented in the sub class tree to use allocator.
	tree_base(const tree_base& other) = delete;

	tree_base(tree_base&& other) :
			_root(std::move(other._root)), _size(other._size) {
		other._size = 0;
	}

	tree_base(std::unique_ptr<node_type> root, size_type s) :
			_root(std::move(root)), _size(s) {
	}

	// Copy logic is implemented in the sub class tree to use allocator.
	tree_base& operator =(const tree_base&) = delete;

	tree_base& operator =(tree_base&& other) {
		_size = other._size;
		other._size = 0;
		_root = std::move(other._root);
		return *this;
	}

public:
	~tree_base() = default;

	bool empty() const {
		return _root == nullptr;
	}

	void clear() {
		_root = nullptr;
		size = 0;
	}

	size_type size() const {
		return _size;
	}

	size_type max_size() const {
		return std::numeric_limits<size_type>::max();
	}

	bool operator ==(const tree_base& other) {
		if (this->_size != other._size) {
			return false;
		}
		if (this->_root) {
			// We are sure that also the other has a _root as we checked _size
			return *this->_root == *other._root;
		}
		return true;
	}

	bool operator !=(const tree_base& other) {
		return !(*this == other);
	}

};

} /* namespace ds */

#endif /* H89E2543D_D005_4E9B_8B4A_C0BDF8AEC5A8 */
