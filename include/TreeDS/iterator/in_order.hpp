#ifndef H53242E79_8449_4605_A949_338CF8B3C51D
#define H53242E79_8449_4605_A949_338CF8B3C51D

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
			return descent_left(*n.right_child());
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
		return descent_left(root);
	}

	template<typename T>
	const node<T>* go_last(const node<T>& root) const {
		return descent_right(root);
	}

}
;

} /* namespace ds */

#endif /* H53242E79_8449_4605_A949_338CF8B3C51D */
