#ifndef H40F7331A_27AB_4B81_A85D_B370D0054AD8
#define H40F7331A_27AB_4B81_A85D_B370D0054AD8

#include "../visitor.hpp"

namespace ds {

class pre_order final : public visitor<pre_order> {

public:
	constexpr pre_order() = default;
	~pre_order() = default;
	template <typename T> T* increment(T&) const;
	template <typename T> T* decrement(T&) const;
	template <typename T> T* go_first(T&) const;
	template <typename T> T* go_last(T&) const;

};

} /* namespace ds */

#include "pre_order.tpp"

#endif /* H40F7331A_27AB_4B81_A85D_B370D0054AD8 */
