#ifndef H0D7AED97_95FE_4380_8923_43A3E5E92A3A
#define H0D7AED97_95FE_4380_8923_43A3E5E92A3A

#include <type_traits>

namespace ds {

template<typename T>
class node;

template<typename T>
const node<T>* cross_bridge_right(const node<T>& n) {
	const node<T>* prev = &n;
	const node<T>* next = n.parent();
	while (next) {
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
	while (next) {
		if (prev != next->first_child()) {
			return next->left_child(); // found
		}
		prev = next;
		next = next->parent();
	}
	return next;
}

/**
 * This function can be used in iterators to keep calling a specific lambda
 * {@link Callable}. The passed type must be convertible to a function that
 * take a reference to constant node and returns a pointer to constant node.
 * The best is to pass a lambda so that it can be inlined.
 */
template<typename T, typename Callable>
const node<T>* descent(const node<T>& n, Callable call) {
	static_assert(
			std::is_convertible<Callable, std::function<const node<T>*(const node<T>&)>>::value,
			"The Callable argument must be of a type that can be called with a"
			"constant node reference and returns a pointer to constant node."
	);
	auto temp = &n;
	auto result = temp;
	do {
		result = temp;
		temp = call(*temp); // called once per iteration, maximum efficiency
	} while (temp);
	return result;
}

} /* namespace ds */

#endif /* H0D7AED97_95FE_4380_8923_43A3E5E92A3A */
