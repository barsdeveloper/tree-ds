#pragma once

namespace md {

class true_matcher {
    public:
    constexpr true_matcher() = default;

    template <typename Value>
    bool match_value(const Value&) {
        return true;
    }
};

} // namespace md
