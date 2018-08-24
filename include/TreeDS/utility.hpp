#pragma once

#define CHECK_CONVERTIBLE(FROM, TO) typename = std::enable_if_t<std::is_convertible_v<FROM, TO>>
#define CHECK_CONSTRUCTIBLE(FROM, TO) typename = std::enable_if_t<std::is_constructible_v<FROM, TO>>

namespace ds {

} // namespace ds
