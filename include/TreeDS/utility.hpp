#pragma once

namespace ds {

#define CHECK_CONVERTIBLE(FROM, TO) typename = std::enable_if_t<std::is_convertible_v<FROM, TO>>
#define CHECK_CONSTRUCTIBLE(FROM, TO) typename = std::enable_if_t<std::is_constructible_v<FROM, TO>>

template <typename T>
struct singleton {
    T& instance;
    template <typename... Args>
    singleton(Args&&... args) :
            instance(singleton::instantiate(std::forward<Args>(args)...)) {
    }
    T& get_instance() {
        return instance;
    }
    template <typename... Args>
    static T& instantiate(Args&&... args) {
        // Unique T across all instances of singleton
        static T instance(std::forward<Args>(args)...);
        return instance;
    }
};

} // namespace ds
