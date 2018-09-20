#pragma once

#include <TreeDS/utility.hpp>

namespace ds {

template <typename T>
class binary_node;

class post_order final {
    public:
    constexpr post_order() = default;

    template <typename T>
    const binary_node<T>* increment(const binary_node<T>& n) const {
        auto prev = &n;
        auto next = n.get_parent();
        if (!next || prev == next->get_last_child()) {
            return next; // found
        }
        return descent(next->get_right_child(), std::mem_fn(&binary_node<T>::get_first_child));
    }

    template <typename T>
    const binary_node<T>* decrement(const binary_node<T>& n) const {
        auto result = n.get_last_child();
        if (result) {
            return result;
        }
        return cross_bridge_left(n);
    }

    template <typename T>
    const binary_node<T>* go_first(const binary_node<T>& root) const {
        return descent(&root, std::mem_fn(&binary_node<T>::get_first_child));
    }

    template <typename T>
    const binary_node<T>* go_last(const binary_node<T>& root) const {
        return &root;
    }
};

} // namespace ds
