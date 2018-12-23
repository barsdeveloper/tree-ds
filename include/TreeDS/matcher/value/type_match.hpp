#pragma once

#include <type_traits>

namespace ds {

template <typename... Types>
class type_match {
    template <typename Value>
    bool match_value(const Value&) {
        return (... || std::is_same_v<Value, Types>);
    }
};

} // namespace ds
