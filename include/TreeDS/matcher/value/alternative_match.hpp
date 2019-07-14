#pragma once

#include <array>
#include <tuple> // std::apply()
#include <typeindex>

namespace md {

template <typename... T>
class alternative_match {

    private:
    std::tuple<T...> alternatives;

    public:
    constexpr alternative_match(const T&... alternatives) :
            alternatives(alternatives...) {
    }

    template <typename Value>
    constexpr bool match_value(const Value& value) {
        return std::apply(
            [&](auto&... values) {
                return (... || (value == values));
            },
            this->alternatives);
    }
};

} // namespace md
