namespace bds {

template <typename T>
tree_base<T>::tree_base(node<T> *root, size_type size) :
		_root(root), _size(size) {
}

template <typename T>
tree_base<T>::tree_base() :
		tree_base(nullptr, 0) {
}

template <typename T>
tree_base<T>::tree_base(tree_base &&other) :
		_root(other._root), _size(other._size) {
	other._root = nullptr;
	other._size = 0;
}

template <typename T>
tree_base<T>& tree_base<T>::operator =(tree_base &&other) {
	_root = other._root;
	_size = other._size;
	other._root = nullptr;
	other._size = 0;
	return *this;
}

template <typename T>
tree_base<T>::~tree_base() {
	delete _root;
}

} /* namespace bds */
