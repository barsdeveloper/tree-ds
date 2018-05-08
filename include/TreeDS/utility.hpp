#pragma once

#define CHECK_CONVERTIBLE(FROM, TO) typename = std::enable_if_t<std::is_convertible_v<FROM, TO>>
#define CHECK_CONSTRUCTIBLE(FROM, TO) typename = std::enable_if_t<std::is_constructible_v<FROM, TO>>

namespace ds {
/*
template <typename A, typename B>
bool both_null_or_not(A* a, B* b) {
    return (a == nullptr) == (b == nullptr);
}
*/
} // namespace ds
