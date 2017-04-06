#ifndef H53242E79_8449_4605_A949_338CF8B3C51D
#define H53242E79_8449_4605_A949_338CF8B3C51D

namespace ds {

template<typename T>
class node;

class pre_order final {

public:
	constexpr pre_order() = default;
	~pre_order() = default;

	template<typename T>
	const node<T>* increment(const node<T>& n) const {
		const node<T>* prev;
		const node<T>* next = &n;
		if (n.right_child()) {
			next = next->right_child;
			prev = next;
			next = next->left_child();
			while (next) {
				prev = next;
				next = next->left_child();
			}
		}
		const node<T>* prev = &n;
		const node<T>* next = n.parent();
		if (!next || prev == next->left_child()) {
			return next;
		}
		next = next->right_child();
		while (next) {
			prev = next;
			next = next->left_child();
		}
		return prev;
	}

	template<typename T>
	const node<T>* decrement(const node<T>& n) const {
		const node<T>* next = n.parent();
		const node<T>* prev = &n;
		if (!next || prev == next->left_child()) {
			return next;
		}
		return next;
	}

	template<typename T> const node<T>* go_first(const node<T>& root) const {
		const node<T>* result = &root;
		while (result->left_child()) {
			result = result->left_child();
		}
		return result;
	}

	template<typename T>
	const node<T>* go_last(const node<T>& root) const {
		const node<T>* result = &root;
		while (result->right_child()) {
			result = result->right_child();
		}
		return result;
	}

};

} /* namespace ds */

#endif /* H53242E79_8449_4605_A949_338CF8B3C51D */
