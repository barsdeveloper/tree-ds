#pragma once

#include <functional>

#include <TreeDS/utility.hpp>

namespace ds {

template <typename T>
class binary_node;

class in_order final {

    public:
    constexpr in_order() = default;

    template <typename T>
    const binary_node<T>* increment(const binary_node<T>& n) const {
        if (n.get_right_child()) {
            return descent(n.get_right_child(), std::mem_fn(&binary_node<T>::get_left_child));
        } else {
            auto prev = &n;
            auto next = n.get_parent();
            while (next) {
                if (prev == next->get_left_child()) {
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
        if (n.get_left_child()) {
            return descent(n.get_left_child(), std::mem_fn(&binary_node<T>::get_right_child));
        }
        auto prev = &n;
        auto next = n.get_parent();
        while (next) {
            if (prev == next->get_right_child()) {
                return next; // found
            }
            prev = next;
            next = next->get_parent();
        }
        return next;
    }

    template <typename T>
    const binary_node<T>* go_first(const binary_node<T>& root) const {
        return descent(&root, std::mem_fn(&binary_node<T>::get_left_child));
    }

    template <typename T>
    const binary_node<T>* go_last(const binary_node<T>& root) const {
        return descent(&root, std::mem_fn(&binary_node<T>::get_right_child));
    }
};

} // namespace ds
