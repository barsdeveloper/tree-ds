#ifndef H0D7AED97_95FE_4380_8923_43A3E5E92A3A
#define H0D7AED97_95FE_4380_8923_43A3E5E92A3A

namespace ds {

template<typename T>
class node;

template<typename T>
const node<T>* cross_bridge_right(const node<T>& n) {
	const node<T>* prev = &n;
	const node<T>* next = n.parent();
	while(next) {
		if (prev != next->last_child()) {
			return next->right_child(); // found
		}
		prev = next;
		next = next->parent();
	}
	return next;
}

template<typename T>
const node<T>* cross_bridge_left(const node<T>& n) {
	const node<T>* prev = &n;
	const node<T>* next = n.parent();
	while(next) {
		if (prev != next->first_child()) {
			return next->left_child(); // found
		}
		prev = next;
		next = next->parent();
	}
	return next;
}

} /* namespace ds */

#endif /* H0D7AED97_95FE_4380_8923_43A3E5E92A3A */
