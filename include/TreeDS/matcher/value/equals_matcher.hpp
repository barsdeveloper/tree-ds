#pragma once

#include <TreeDS/matcher/matchable.hpp>

namespace ds {

template <typename T>
class equals_matcher {

    T comparison;

    public:
    equals_matcher(const T& comparison) :
            comparison(comparison) {
    }

    template <typename Value>
    bool match_value(const Value& target) {
        return comparison == target;
    }
};

} // namespace ds
