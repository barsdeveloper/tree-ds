#pragma once

#include <type_traits>

namespace ds {

template <typename... Types>
class type_match {

    template <typename Matchable>
    bool match(const Matchable& m) {
        if constexpr ((std::is_same_v<Matchable, Types> || ...)) {
            this->set_matched(&m);
            return true;
        } else {
            return false;
        }
    }
};

} // namespace ds
