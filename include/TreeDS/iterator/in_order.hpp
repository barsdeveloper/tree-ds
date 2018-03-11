#pragma once

#include "helper.hpp"

namespace ds {

template <typename T>
class binary_node;

class in_order final {

public:
    constexpr in_order() = default;

    template <typename T>
    const binary_node<T>* increment(const binary_node<T>& n) const {
        if (n.right_child()) {
            return descent(*n.right_child(), [](const binary_node<T>& n) {
                return n.left_child();
            });
        }
        auto prev = &n;
        auto next = n.parent();
        while (next) {
            if (prev == next->left_child()) {
                return next; // found
            }
            prev = next;
            next = next->parent();
        }
        return next;
    }

    template <typename T>
    const binary_node<T>* decrement(const binary_node<T>& n) const {
        if (n.left_child()) {
            return descent_right(*n.left_child());
        }
        auto prev = &n;
        auto next = n.parent();
        while (next) {
            if (prev == next->right_child()) {
                return next; // found
            }
            prev = next;
            next = next->parent();
        }
        return next;
    }

    template <typename T>
    const binary_node<T>* go_first(const binary_node<T>& root) const {
        return descent(root, [](const binary_node<T>& n) {
            return n.left_child();
        });
    }

    template <typename T>
    const binary_node<T>* go_last(const binary_node<T>& root) const {
        return descent(root, [](const binary_node<T>& n) {
            return n.right_child();
        });
    }
};

} // namespace ds
