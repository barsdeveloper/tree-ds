#ifndef PRE_ORDER_HPP_
#define PRE_ORDER_HPP_

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

#endif /* PRE_ORDER_HPP_ */
