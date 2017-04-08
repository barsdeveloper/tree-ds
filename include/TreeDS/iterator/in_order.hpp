#ifndef H53242E79_8449_4605_A949_338CF8B3C51D
#define H53242E79_8449_4605_A949_338CF8B3C51D

#include "helper.hpp"

namespace ds {

template<typename T>
class node;

class in_order final {

public:
	constexpr in_order() = default;
	~in_order() = default;

	template<typename T>
	const node<T>* increment(const node<T>& n) const {
		if (n.right_child()) {
			return descent(*n.right_child(), [](const node<T>& n) {
				return n.left_child();
			});
		}
		auto prev = &n;
		auto next = n.parent();
		while (next) {
			if (prev == next->left_child()) {
				return next; // found
			}
			prev = next;
			next = next->parent();
		}
		return next;
	}

	template<typename T>
	const node<T>* decrement(const node<T>& n) const {
		if (n.left_child()) {
			return descent_right(*n.left_child());
		}
		auto prev = &n;
		auto next = n.parent();
		while (next) {
			if (prev == next->right_child()) {
				return next; // found
			}
			prev = next;
			next = next->parent();
		}
		return next;
	}

	template<typename T> const node<T>* go_first(const node<T>& root) const {
		return descent(root, [](const node<T>& n) {
			return n.left_child();
		});
	}

	template<typename T>
	const node<T>* go_last(const node<T>& root) const {
		return descent(root, [](const node<T>& n) {
			return n.right_child();
		});
	}

}
;

} /* namespace ds */

#endif /* H53242E79_8449_4605_A949_338CF8B3C51D */
