#include "../../node/node.hpp"
#include "../algorithm/algorithm.hpp"

namespace bds {

template <typename T, typename A, bool C>
constexpr tree_iterator<T, A, C>::tree_iterator() :
		_algorithm(A::get_instance()), _tree(nullptr), _node(nullptr) {
}

template <typename T, typename A, bool C>
tree_iterator<T, A, C>::tree_iterator(const tree_iterator &other) :
		_algorithm(A::get_instance()), _tree(other._tree), _node(other._node) {
}

template <typename T, typename A, bool C>
tree_iterator<T, A, C>& tree_iterator<T, A, C>::operator =(const tree_iterator &other) {
	_tree = other._tree;
	_node = other._node;
	return *this;
}

template <typename T, typename A, bool C>
tree_iterator<T, A, C>::tree_iterator(tree_type &tree) :
		_algorithm(A::get_instance()), _tree(&tree), _node(nullptr) {
}

template <typename T, typename A, bool C>
tree_iterator<T, A, C>::tree_iterator(tree_type &tree, node_type &current) :
		_algorithm(A::get_instance()), _tree(&tree), _node(&current) {
}

template <typename T, typename A, bool C>
template <typename, typename> tree_iterator<T, A, C>::tree_iterator(const tree_iterator<T, A, false> &iterator) :
		_algorithm(A::get_instance()), _tree(iterator._tree), _node(iterator._node) {
}

template <typename T, typename A, bool C>
typename tree_iterator<T, A, C>::node_type* tree_iterator<T, A, C>::node() const {
	return _node;
}

template <typename T, typename A, bool C>
typename tree_iterator<T, A, C>::value_type& tree_iterator<T, A, C>::operator *() const {
	return _node->_argument;
}

template <typename T, typename A, bool C>
typename tree_iterator<T, A, C>::value_type* tree_iterator<T, A, C>::operator ->() const {
	return &(_node->_argument);
}

template <typename T, typename A, bool C>
bool tree_iterator<T, A, C>::operator ==(const tree_iterator& other) const {
	return {
		_node == other._node
	};
}

template <typename T, typename A, bool C>
bool tree_iterator<T, A, C>::operator !=(const tree_iterator& other) const {
	return !(*this == other);
}

template <typename T, typename A, bool C>
tree_iterator<T, A, C> & tree_iterator<T, A, C>::operator ++() {
	if (_node) {
		_node = static_cast<node_type*>(_algorithm.increment(*_node));
	} else if (_tree && _tree->_root) { // If iterator is at the end() (REMEMBER: end()._node == nullptr)
		// normal iterator  => incremented from end() => go to its first element (rewind)
		// reverse iterator	=> decremented from end() => go to its last element (before end())
		// REMEMBER: ++ operator on a reverse_iterator delegates to -- operator of tree_iterator and vice versa
		_node = static_cast<node_type*>(_algorithm.go_first(*_tree->_root));
	}
	return *this;
}

template <typename T, typename A, bool C>
tree_iterator<T, A, C> tree_iterator<T, A, C>::operator ++(int) {
	tree_iterator it(*this);
	++(*this);
	return it;
}

template <typename T, typename A, bool C>
tree_iterator<T, A, C>& tree_iterator<T, A, C>::operator --() {
	if (_node) {
		node_type *temp = _node;
		_node = static_cast<node_type*>(_algorithm.decrement(*_node));
		if (!_node) { // if decrementation caused past the begin() ...
			_node = temp; // ... then leave _node as it was
		}
	} else if (_tree && _tree->_root) { // if iterator is at the end() (REMEMBER: end() => _node == nullptr)
		// normal iterator  => decremented from end() => go to its last element (before end())
		// reverse iterator => incremented from end() => go to its first element (rewind)
		// REMEMBER: ++ operator on a reverse_iterator delegates to -- operator of tree_iterator and vice versa
		_node = static_cast<node_type*>(_algorithm.go_last(*_tree->_root));
	}
	return *this;
}

template <typename T, typename A, bool C>
tree_iterator<T, A, C> tree_iterator<T, A, C>::operator --(int) {
	tree_iterator it(*this);
	--(*this);
	return it;
}

} /* namespace bds */
