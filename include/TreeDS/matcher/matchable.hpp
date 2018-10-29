#pragma once

namespace ds {

template <typename ActualMatchable>
class matchable {
    public:
    ~matchable()                             = default;
    bool accept(type_matcher& matcher) const = 0;
};

} // namespace ds
