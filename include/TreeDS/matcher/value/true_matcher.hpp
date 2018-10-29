#pragma once

namespace ds {

class true_matcher {
    public:
    template <typename ActualMatchable>
    bool match(const ActualMatchable&) {
        return true;
    }
};

} // namespace ds
