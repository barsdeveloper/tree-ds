namespace ds {

template <typename T>
T* pre_order::increment(T &node) const {
	T *result = &node;
	if(result->_first_child) {
		result = result->_first_child;
	} else {
		/*
		 * If the current node has no children, go up until find an ancestor that has a next
		 * sibling node or until you pass the root of the tree.
		 */
		while(result && !result->_next_sibling) {
			result = result->_parent;
		}
		if(result) result = result->_next_sibling; // if reached root, this will be nullptr
	}
	return result;
}

template <typename T>
T* pre_order::decrement(T &node) const {
	T *result = &node;
	if(result->_prev_sibling) {
		/*
		 * If the current node has a previous sibling, the next node will be lowest last
		 * children of this previous sibling.
		 */
		result = result->_prev_sibling;
		while(result->_last_child) {
			result = result->_last_child;
		}
	} else {
		result = result->_parent; // if reached root, this will be nullptr
	}
	return result;
}

template <typename T>
T* pre_order::go_first(T &root) const {
	T *result = &root;
	return result;
}

template <typename T>
T* pre_order::go_last(T &root) const {
	T *result = &root;
	while(result->_last_child) {
		result = result->_last_child; // go to the last child
	}
	return result;
}

} /* namespace ds */
