#pragma once

#include <functional>

#include <TreeDS/utility.hpp>

namespace md {

namespace binary {
    template <typename T>
    class node;
}

class in_order final {

    public:
    constexpr in_order() = default;

    template <typename T>
    const binary::node<T>* increment(const binary::node<T>& from) {
        if (from.get_right_child()) {
            return keep_calling(*from.get_right_child(), std::mem_fn(&binary::node<T>::get_left_child));
        } else {
            const binary::node<T>* prev = &from;
            const binary::node<T>* next = from.get_parent();
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
    const binary::node<T>* decrement(const binary::node<T>& from) {
        if (from.get_left_child()) {
            return keep_calling(*from.get_left_child(), std::mem_fn(&binary::node<T>::get_right_child));
        }
        const binary::node<T>* prev = &from;
        const binary::node<T>* next = from.get_parent();
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
    const binary::node<T>* go_first(const binary::node<T>& root) {
        return keep_calling(root, std::mem_fn(&binary::node<T>::get_left_child));
    }

    template <typename T>
    const binary::node<T>* go_last(const binary::node<T>& root) {
        return keep_calling(root, std::mem_fn(&binary::node<T>::get_right_child));
    }
};

} // namespace md
