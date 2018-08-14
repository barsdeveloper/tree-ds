#pragma once

#define CHECK_CONVERTIBLE(FROM, TO) typename = std::enable_if_t<std::is_convertible_v<FROM, TO>>
#define CHECK_CONSTRUCTIBLE(FROM, TO) typename = std::enable_if_t<std::is_constructible_v<FROM, TO>>

namespace ds {

template <typename T, typename V, size_t... I>
void visit_impl(T&& t, V&& v, std::index_sequence<I...>) {
    (..., v(std::get<I>(t)));
}

template <typename T, typename V>
void visit(T&& t, V&& v) {
    visit_impl(std::forward<T>(t), std::forward<V>(v), std::make_index_sequence<std::tuple_size<typename std::decay<T>::type>::value>());
}

} // namespace ds
