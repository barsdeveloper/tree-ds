#pragma once

#include <functional>

#include <TreeDS/utility.hpp>

namespace md {

template <typename T>
class binary_node;

class in_order final {

    public:
    constexpr in_order() = default;

    template <typename T>
    const binary_node<T>* increment(const binary_node<T>& from) {
        if (from.get_right_child()) {
            return keep_calling(
                // from
                *from.get_right_child(),
                // keep calling
                std::mem_fn(&binary_node<T>::get_left_child));
        } else {
            bool found                   = false;
            const binary_node<T>* result = keep_calling(
                // from
                from,
                // keep calling
                std::mem_fn(&binary_node<T>::get_parent),
                // until
                [](const binary_node<T>& child, const binary_node<T>& parent) {
                    return &child == parent.get_left_child();
                },
                // then return
                [&](const binary_node<T>&, const binary_node<T>& parent) {
                    found = true;
                    return &parent;
                });
            return found ? result : nullptr;
        }
    }

    template <typename T>
    const binary_node<T>* decrement(const binary_node<T>& from) {
        if (from.get_left_child()) {
            return keep_calling(*from.get_left_child(), std::mem_fn(&binary_node<T>::get_right_child));
        }
        return keep_calling(
            // from
            from,
            // keep calling
            std::mem_fn(&binary_node<T>::get_parent),
            // until
            [](const binary_node<T>& child, const binary_node<T>& parent) {
                return &child == parent.get_right_child();
            },
            // then return
            [](const binary_node<T>&, const binary_node<T>& parent) {
                return &parent;
            });
    }

    template <typename T>
    const binary_node<T>* go_first(const binary_node<T>& root) {
        return keep_calling(root, std::mem_fn(&binary_node<T>::get_left_child));
    }

    template <typename T>
    const binary_node<T>* go_last(const binary_node<T>& root) {
        return keep_calling(root, std::mem_fn(&binary_node<T>::get_right_child));
    }
};

} // namespace md
