#pragma once

#define CHECK_CONVERTIBLE_T typename = std::enable_if_t<std::is_convertible_v<ConvertibleT, T>>
#define CHECK_CONSTRUCTIBLE_T typename = std::enable_if_t<std::is_constructible_v<T, ConstructibleT...>>

namespace ds {
/*
template <typename A, typename B>
bool both_null_or_not(A* a, B* b) {
    return (a == nullptr) == (b == nullptr);
}
*/
} // namespace ds
