#pragma once

#include <type_traits>

namespace ds {

template <typename... Types>
class type_match {
    template <typename ActualMatchable>
    bool match_value(const ActualMatchable&) {
        return (... || std::is_same_v<ActualMatchable, Types>);
    }
};

} // namespace ds
