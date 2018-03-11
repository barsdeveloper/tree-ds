#pragma once

#include "helper.hpp"

namespace ds {

template <typename T>
class binary_node;

class post_order final {
public:
    constexpr post_order() = default;
    ~post_order()          = default;

    template <typename T>
    const binary_node<T>* increment(const binary_node<T>& n) const {
        auto prev = &n;
        auto next = n.parent();
        if (!next || prev == next->last_child()) {
            return next; // found
        }
        return descent(*next->right_child(), [](const binary_node<T>& n) { return n.first_child(); });
    }

    template <typename T>
    const binary_node<T>* decrement(const binary_node<T>& n) const {
        auto result = n.last_child();
        if (result) {
            return result;
        }
        return cross_bridge_left(n);
    }

    template <typename T>
    const binary_node<T>* go_first(const binary_node<T>& root) const {
        return descent(root, [](const binary_node<T>& n) { return n.first_child(); });
    }

    template <typename T>
    const binary_node<T>* go_last(const binary_node<T>& root) const {
        return &root;
    }
};

} // namespace ds
