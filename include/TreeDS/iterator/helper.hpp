#pragma once

#include <functional>
#include <type_traits>

namespace ds {

template <typename T>
class binary_node;

template <typename T>
const binary_node<T>* cross_bridge_right(const binary_node<T>& n) {
    const binary_node<T>* prev = &n;
    const binary_node<T>* next = n.get_parent();
    while (next) {
        if (prev != next->last_child()) {
            return next->get_right(); // found
        }
        prev = next;
        next = next->get_parent();
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

template <typename Node, typename Call, typename Test, typename Result>
const Node* descent(const Node* from, Call call, Test test, Result result) {
    const Node* prev = from;
    const Node* next = call(from);
    while (next != nullptr) {
        if (test(prev, next)) {
            return result(next);
        }
        next = prev;
        next = call(prev);
    }
    return prev;
}

/**
 * This function can be used in iterators to keep calling a specific lambda {@link Callable}. The passed type must be
 * convertible to a function that take a reference to constant node and returns a pointer to constant node. The best is
 * to pass a lambda so that it can be inlined.
 */
template <typename Node, typename Callable>
const Node* descent(const Node* from, Callable call) {
    const Node* prev = from;
    const Node* next = call(from);
    while (next != nullptr) {
        prev = next;
        next = call(next);
    }
    return prev;
}

} // namespace ds
