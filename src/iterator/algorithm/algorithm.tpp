namespace bds {

template <typename T>
const T& algorithm<T>::get_instance() {
	static constexpr T instance{};
	return instance;
}

} /* namespace bds */
