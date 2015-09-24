#include <limits> // std::numeric_limits
#include <stdexcept> // std::invalid_argument
#include <type_traits> // std::is_same

namespace ds {

template <typename T, typename Alg, typename Alloc>
template <typename It> typename tree<T, Alg, Alloc>::node_type* tree<T, Alg, Alloc>::extract_node(It it) {
	/*
	 * According to C++ STL, containers' methods that has an iterator type argument, only accept const_iterator type to
	 * indicate the position. Remember that every iterator type is implicitly convertible to const_iterator type and
	 * every reverse iterator has a base() method that returns the normal iterator (const or non const depending on the
	 * type) REMEMBER: base() does not return an iterator pointing to the same element but (from its perspective) to the
	 * next element (so rbegin.base() == end() and rend.base() == begin())
	 */
	static_assert(
			!std::is_same<It, iterator<typename It::algorithm_type>>::value &&
			!std::is_same<It, const_iterator<typename It::algorithm_type>>::value,
			"Expected tree::iterator or tree::reverse_iterator type");
	if (this != it._tree) throw std::invalid_argument("iterator does not refer to this tree");
	// Iterator refers to not empty tree but points to the end.
	if (!it._node && static_cast<const tree*>(it._tree)->_root) {
		/*
		 * Static cast is needed as sub classes cannot access protected members of its superclass through another
		 * instance than itself (this). Casting to this type is safe as it is proved that this == it._tree.
		 */
		throw std::invalid_argument("iterator points to the end (not allowed for this type of container)");
	}
	/*
	 * I dislike const_cast, but have to use it as STL specify to take const_iterator as input for every method of
	 * container that need to take a position. Anyway, it can be used safely in this context because iterator refers
	 * to this tree (which is NOT const).
	 */
	return const_cast<node_type*>(it._node); // position
}

template <typename T, typename Alg, typename Alloc>
tree<T, Alg, Alloc>::tree(const tree &other) :
		tree_base<T>(new node_type(*other._root), other._size) { // TODO: use allocators
}

template <typename T, typename Alg, typename Alloc>
tree<T, Alg, Alloc>& tree<T, Alg, Alloc>::operator =(const tree &other) {
	this->_root = new node_type(*other._root); // TODO: use allocators
	this->_size = other._size;
	return *this;
}

/*   ---   begin   ---   **********************************************************************************************/

template <typename T, typename Alg, typename Alloc>
template <typename A> tree<T, Alg, Alloc>::iterator<A> tree<T, Alg, Alloc>::begin() {
	/*
	 * What I do here is: Create a new instance of the iterator that will have the attribute _node (current node) set
	 * to nullptr. That means the iterator is at the end. Now, incrementing an iterator at the end cause the iterator
	 * to point to its first pointed node (discovered by accident). See tree_iterator::operator ++() and operator --()
	 */
	return ++(iterator<A>(*this));
}

template <typename T, typename Alg, typename Alloc>
template <typename A> tree<T, Alg, Alloc>::const_iterator<A> tree<T, Alg, Alloc>::begin() const {
	return cbegin();
}

template <typename T, typename Alg, typename Alloc>
template <typename A> tree<T, Alg, Alloc>::const_iterator<A> tree<T, Alg, Alloc>::cbegin() const {
	/*
	 * What I do here is: Create a new instance of the iterator that will have the attribute _node (current node) set
	 * to nullptr. That means the iterator is at the end. Now, incrementing an iterator at the end cause the iterator
	 * to point to its first pointed node (discovered by accident). See tree_iterator::operator ++() and operator --()
	 */
	return ++(const_iterator<A>(*this));
}

/*   ---   end   ---   ************************************************************************************************/

template <typename T, typename Alg, typename Alloc>
template <typename A> tree<T, Alg, Alloc>::iterator<A> tree<T, Alg, Alloc>::end() {
	return iterator<A>(*this);
}

template <typename T, typename Alg, typename Alloc>
template <typename A> tree<T, Alg, Alloc>::const_iterator<A> tree<T, Alg, Alloc>::end() const {
	return cend();
}

template <typename T, typename Alg, typename Alloc>
template <typename A> tree<T, Alg, Alloc>::const_iterator<A> tree<T, Alg, Alloc>::cend() const {
	return const_iterator<A>(*this);
}

/*   ---   reverse begin   ---   **************************************************************************************/

template <typename T, typename Alg, typename Alloc>
template <typename A> tree<T, Alg, Alloc>::reverse_iterator<A> tree<T, Alg, Alloc>::rbegin() {
	/*
	 * What I do here is: Create a new instance of the iterator that will have the attribute _node (current node) set
	 * to nullptr. That means the iterator is at the end. Now, incrementing an iterator at the end cause the iterator
	 * to point to its first pointed node (discovered by accident). See tree_iterator::operator ++() and operator --()
	 */
	return ++(reverse_iterator<A>(*this));
}

template <typename T, typename Alg, typename Alloc>
template <typename A> tree<T, Alg, Alloc>::const_reverse_iterator<A> tree<T, Alg, Alloc>::rbegin() const {
	return crbegin();
}

template <typename T, typename Alg, typename Alloc>
template <typename A> tree<T, Alg, Alloc>::const_reverse_iterator<A> tree<T, Alg, Alloc>::crbegin() const {
	/*
	 * What I do here is: Create a new instance of the iterator that will have the attribute _node (current node) set
	 * to nullptr. That means the iterator is at the end. Now, incrementing an iterator at the end cause the iterator
	 * to point to its first pointed node (discovered by accident). See tree_iterator::operator ++() and operator --()
	 */
	return ++(const_reverse_iterator<A>(*this));
}

/*   ---   reverse end   ---   ****************************************************************************************/

template <typename T, typename Alg, typename Alloc>
template <typename A> tree<T, Alg, Alloc>::reverse_iterator<A> tree<T, Alg, Alloc>::rend() {
	return reverse_iterator<A>(*this);
}

template <typename T, typename Alg, typename Alloc>
template <typename A> tree<T, Alg, Alloc>::const_reverse_iterator<A> tree<T, Alg, Alloc>::rend() const {
	return crend();
}

template <typename T, typename Alg, typename Alloc>
template <typename A> tree<T, Alg, Alloc>::const_reverse_iterator<A> tree<T, Alg, Alloc>::crend() const {
	return const_reverse_iterator<A>(*this);
}

/*   ---      ---   ***************************************************************************************************/

template <typename T, typename Alg, typename Alloc>
bool tree<T, Alg, Alloc>::empty() const {
	return this->_root != nullptr;
}

template <typename T, typename Alg, typename Alloc>
typename tree<T, Alg, Alloc>::size_type tree<T, Alg, Alloc>::size() const {
	return this->_size;
}

template <typename T, typename Alg, typename Alloc>
typename tree<T, Alg, Alloc>::size_type tree<T, Alg, Alloc>::max_size() const {
	return std::numeric_limits<size_type>::max();
}

template <typename T, typename Alg, typename Alloc>
void tree<T, Alg, Alloc>::clear() {
	if (this->_root) {
		delete this->_root;
		this->_root = nullptr;
	}
}

template <typename T, typename Alg, typename Alloc>
template <typename It>
tree<T, Alg, Alloc>::iterator<typename It::algorithm_type> tree<T, Alg, Alloc>::insert(It pos,
		const value_type &value) {
	return emplace(pos, value);
}

template <typename T, typename Alg, typename Alloc>
template <typename It>
tree<T, Alg, Alloc>::iterator<typename It::algorithm_type> tree<T, Alg, Alloc>::insert(It pos, value_type &&value) {
	return emplace(pos, std::move(value));
}

template <typename T, typename Alg, typename Alloc>
template <typename It>
tree<T, Alg, Alloc>::iterator<typename It::algorithm_type> tree<T, Alg, Alloc>::append_child(It pos,
		const value_type &value) {
	return emplace_append_child(pos, value);
}

template <typename T, typename Alg, typename Alloc>
template <typename It>
tree<T, Alg, Alloc>::iterator<typename It::algorithm_type> tree<T, Alg, Alloc>::append_child(It pos,
		value_type &&value) {
	return emplace_append_child(pos, std::move(value));
}

template <typename T, typename Alg, typename Alloc>
template <typename It>
tree<T, Alg, Alloc>::iterator<typename It::algorithm_type> tree<T, Alg, Alloc>::append_child(It pos,
		const tree &value) {
	return emplace_append_child(pos, tree);
}

template <typename T, typename Alg, typename Alloc>
template <typename It>
tree<T, Alg, Alloc>::iterator<typename It::algorithm_type> tree<T, Alg, Alloc>::append_child(It pos, tree &&tree) {
	return emplace_append_child(pos, std::move(tree._root));
}

template <typename T, typename Alg, typename Alloc>
template <typename It>
tree<T, Alg, Alloc>::iterator<typename It::algorithm_type> tree<T, Alg, Alloc>::prepend_child(It pos,
		const value_type &value) {
	return emplace_prepend_child(pos, value);
}

template <typename T, typename Alg, typename Alloc>
template <typename It>
tree<T, Alg, Alloc>::iterator<typename It::algorithm_type> tree<T, Alg, Alloc>::prepend_child(It pos,
		value_type &&value) {
	return emplace_prepend_child(pos, std::move(value));
}

template <typename T, typename Alg, typename Alloc>
template <typename It, typename ...Args>
tree<T, Alg, Alloc>::iterator<typename It::algorithm_type> tree<T, Alg, Alloc>::emplace(It pos, Args&& ...args) {
	node_type *n = new node_type(args...);
	node_type *p = extract_node(pos);
	if (p) p->insert(*n);
	else this->_root = n;
	return iterator<typename It::algorithm_type>(*this, *n);
}

template <typename T, typename Alg, typename Alloc>
template <typename It, typename ...Args>
tree<T, Alg, Alloc>::iterator<typename It::algorithm_type> tree<T, Alg, Alloc>::emplace_append_child(It pos,
		Args&& ...args) {
	node_type *n = new node_type(args...);
	node_type *p = extract_node(pos);
	if (p) p->append_child(*n);
	else this->_root = n;
	return iterator<typename It::algorithm_type>(*this, *n);
}

template <typename T, typename Alg, typename Alloc>
template <typename It, typename ...Args>
tree<T, Alg, Alloc>::iterator<typename It::algorithm_type> tree<T, Alg, Alloc>::emplace_prepend_child(It pos,
		Args&& ...args) {
	node_type *n = new node_type(args...);
	node_type *p = extract_node(pos);
	if (p) p->prepend_child(*n);
	else this->_root = n;
	return iterator<typename It::algorithm_type>(*this, *n);
}

} /* namespace ds */
