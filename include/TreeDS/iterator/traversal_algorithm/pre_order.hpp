#ifndef H40F7331A_27AB_4B81_A85D_B370D0054AD8
#define H40F7331A_27AB_4B81_A85D_B370D0054AD8

#include "../visitor.hpp"

namespace ds {

template <typename> class node;

class pre_order final : public visitor<pre_order> {

public:
	constexpr pre_order() = default;
	~pre_order() = default;
	template <typename T> const node<T>* increment(const node<T>&) const;
	template <typename T> const node<T>* decrement(const node<T>&) const;
	template <typename T> const node<T>* go_first(const node<T>&) const;
	template <typename T> const node<T>* go_last(const node<T>&) const;

};

} /* namespace ds */

#include "../traversal_algorithm/pre_order.tpp"

#endif /* H40F7331A_27AB_4B81_A85D_B370D0054AD8 */
