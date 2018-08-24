#pragma once

#include <functional>
#include "helper.hpp"

namespace ds {

template <typename T>
class binary_node;

class in_order final {

    public:
    constexpr in_order() = default;

    template <typename T>
    const binary_node<T>* increment(const binary_node<T>& n) const {
        if (n.get_right()) {
            return descent(*n.get_right(), [](const binary_node<T>& n) {
                return n.get_left();
            });
        } else {
            auto prev = &n;
            auto next = n.get_parent();
            while (next) {
                if (prev == next->get_left()) {
                    return next; // found
                }
                prev = next;
                next = next->get_parent();
            }
            return next;
        }
    }

    template <typename T>
    const binary_node<T>* decrement(const binary_node<T>& n) const {
        if (n.get_left()) {
            return descent_right(*n.get_left());
        }
        auto prev = &n;
        auto next = n.parent();
        while (next) {
            if (prev == next->get_right()) {
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
            return n.get_left();
        });
    }

    template <typename T>
    const binary_node<T>* go_last(const binary_node<T>& root) const {
        return descent(root, [](const binary_node<T>& n) {
            return n.get_right();
        });
    }
};

} // namespace ds
