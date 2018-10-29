#pragma once

namespace ds {

class false_matcher {
    public:
    template <typename ActualMatchable>
    bool match(const ActualMatchable&) {
        return false;
    }
};

} // namespace ds
