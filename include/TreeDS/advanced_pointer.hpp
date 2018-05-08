#pragma once

#include <atomic>
#include <variant>

namespace ds {

template <typename T>
struct pointer_getter {
    T* operator()(const std::atomic<T*>& pointer) {
        return pointer.load(std::memory_order_relaxed);
    }
    T* operator()(const T*& pointer_reference) {
        return pointer_reference;
    }
};

template <typename T>
struct pointer_setter {
    T* value;
    pointer_setter(T* value) :
            value(value) {
    }
    void operator()(std::atomic<T*>& pointer_reference) {
        pointer_reference.store(value, std::memory_order_relaxed);
    }
    void operator()(T*& pointer_reference) {
        pointer_reference = value;
    }
};

template <typename T, bool Atomic = false, bool SetOnce = false>
class advanced_pointer {

    using pointer_t = std::variant<T*, std::atomic<T*>>;

private:
    pointer_t pointer;

public:
    advanced_pointer() :
            pointer(nullptr) {
    }
    T* get() const {
        return std::visit(pointer_getter<T>{}, pointer);
    }

    void set(T* value) {
        std::visit(pointer_setter<T>{value}, pointer);
    }
};

} // namespace ds
