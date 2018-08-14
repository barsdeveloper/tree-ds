#pragma once

#include <functional>
#include <type_traits>

namespace ds {

template <typename T>
class binary_node;

template <typename T>
const binary_node<T>* cross_bridge_right(const binary_node<T>& n) {
    const binary_node<T>* prev = &n;
    const binary_node<T>* next = n.parent();
    while (next) {
        if (prev != next->last_child()) {
            return next->right_child(); // found
        }
        prev = next;
        next = next->parent();
    }
    return next;
}

template <typename T>
const binary_node<T>* cross_bridge_left(const binary_node<T>& n) {
    const binary_node<T>* prev = &n;
    const binary_node<T>* next = n.parent();
    while (next) {
        if (prev != next->first_child()) {
            return next->left_child(); // found
        }
        prev = next;
        next = next->parent();
    }
    return next;
}

/**
 * This function can be used in iterators to keep calling a specific lambda {@link Callable}. The passed type must be
 * convertible to a function that take a reference to constant node and returns a pointer to constant node. The best is
 * to pass a lambda so that it can be inlined.
 */
template <typename T, typename Callable>
const binary_node<T>* descent(const binary_node<T>& n, Callable call) {
    static_assert(
        std::is_convertible<
            Callable, std::function<const binary_node<T>*(const binary_node<T>&)>>::value,
        "The Callable argument must be of a type that can be called with a constant node reference and returns a pointer"
        " to constant node.");
    auto temp   = &n;
    auto result = temp;
    do {
        result = temp;
        temp   = call(*temp);
    } while (temp);
    return result;
}

} // namespace ds
