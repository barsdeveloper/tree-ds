namespace ds {

template <typename> class node;

template <typename T>
const node<T>* pre_order::increment(const node<T> &n) const {
	const node<T> *result = &n;
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
const node<T>* pre_order::decrement(const node<T> &n) const {
	const node<T> *result = &n;
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
const node<T>* pre_order::go_first(const node<T> &root) const {
	const node<T> *result = &root;
	return result;
}

template <typename T>
const node<T>* pre_order::go_last(const node<T> &root) const {
	const node<T> *result = &root;
	while(result->last_child()) {
		result = result->last_child(); // go to the last child
	}
	return result;
}

} /* namespace ds */
