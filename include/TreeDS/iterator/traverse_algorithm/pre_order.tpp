namespace ds {

template <typename T>
T* pre_order::increment(T &node) const {
	T *result = &node;
	if(result->first_child()) {
		result = result->first_child();
	} else {
		/*
		 * If the current node has no children, go up until find an ancestor that has a next
		 * sibling node or until you pass the root of the tree.
		 */
		while(result && !result->next_sibling()) {
			result = result->parent();
		}
		if(result) result = result->next_sibling(); // if reached root, this will be nullptr
	}
	return result;
}

template <typename T>
T* pre_order::decrement(T &node) const {
	T *result = &node;
	if(result->prev_sibling()) {
		/*
		 * If the current node has a previous sibling, the next node will be lowest last
		 * children of this previous sibling.
		 */
		result = result->prev_sibling();
		while(result->last_child()) {
			result = result->last_child();
		}
	} else {
		result = result->parent(); // if reached root, this will be nullptr
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
	while(result->last_child()) {
		result = result->last_child(); // go to the last child
	}
	return result;
}

} /* namespace ds */
