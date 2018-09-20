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

template <typename Node, typename Call, typename Test, typename Result>
const Node* descent(const Node* from, Call call, Test test, Result result) {
    const Node* prev = nullptr;
    const Node* next = from;
    while (next != nullptr) {
        if (test(prev, next)) {
            return result(prev, next);
        }
        prev = next;
        next = call(next);
    }
    return prev;
}

/**
 * This function can be used to keep calling a specific lambda {@link Callable}. The passed type must be convertible to
 * a function that take a reference to constant node and returns a pointer to constant node. The best is to pass a
 * lambda so that it can be inlined.
 *
 * The case from == nullptr is correctly managed.
 */
template <typename Node, typename Callable>
const Node* descent(const Node* from, Callable call) {
    const Node* prev = nullptr;
    const Node* next = from;
    while (next != nullptr) {
        prev = next;
        next = call(next);
    }
    return prev;
}

template <typename>
class binary_node;

template <typename Node>
const Node* cross_bridge_right(const Node& n) {
    const Node* prev = &n;
    const Node* next = n.get_parent();
    while (next) {
        if (prev != next->get_last_child()) {
            return prev->get_next_sibling(); // found
        }
        prev = next;
        next = next->get_parent();
    }
    return next;
}

template <typename Node>
const Node* cross_bridge_left(const Node& n) {
    const Node* prev = &n;
    const Node* next = n.get_parent();
    while (next) {
        if (prev != next->get_first_child()) {
            return next->get_left_child(); // found
        }
        prev = next;
        next = next->get_parent();
    }
    return next;
}

} // namespace ds
