#pragma once

namespace md {

class true_matcher {

    public:
    constexpr true_matcher() {
    }

    template <typename Value>
    constexpr bool match_value(const Value&) {
        return true;
    }
};

true_matcher with_true() {
    return true_matcher();
}

} // namespace md
