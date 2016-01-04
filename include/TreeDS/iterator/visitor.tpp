namespace ds {

template <typename T>
const T& visitor<T>::get_instance() {
	static constexpr T instance{};
	return instance;
}

} /* namespace ds */
