#pragma once

namespace md {

template <typename T, typename Func>
class product_match {

    T artifact;
    Func getter;

    public:
    constexpr product_match(const T& artifact, const Func& getter) :
            artifact(artifact),
            getter(getter) {
    }

    template <typename Value>
    constexpr bool match_value(const Value& value) {
        return this->artifact == getter(value);
    }
};

template <typename T, typename Func>
product_match<T, Func> having(const T& artifact, const Func& getter) {
    return product_match<T, Func>(artifact, getter);
}

} // namespace md
