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

constexpr true_matcher true_match = true_matcher();

} // namespace md
