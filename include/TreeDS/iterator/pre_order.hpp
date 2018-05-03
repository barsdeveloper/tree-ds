#pragma once

#include "helper.hpp"

namespace ds {

template <typename T>
class binary_node;

class pre_order final {

public:
    constexpr pre_order() = default;

    template <typename T>
    const binary_node<T>* increment(const binary_node<T>& node) const {
        auto result = node.first_child();
        if (result) {
            return result;
        }
        return cross_bridge_right(node);
    }

    template <typename T>
    const binary_node<T>* decrement(const binary_node<T>& node) const {
        const binary_node<T>* next = node.parent();
        const binary_node<T>* prev = &node;
        /*
		 * The parent is the next node if (REMEMBER: we traverse tree in pre-order and decrement the iterator):
		 *     1) The passed node is root (its parent is nullptr so the previous value is the end of the reverse iterator).
		 *     2) The node is the unique child of its parent
		 */
        if (!next || prev == next->first_child()) {
            return next;
        }
        return descent(*next->left_child(), [](const binary_node<T>& node) {
            return node.last_child();
        });
    }

    template <typename T>
    const binary_node<T>* go_first(const binary_node<T>& root) const {
        return &root;
    }

    template <typename T>
    const binary_node<T>* go_last(const binary_node<T>& root) const {
        return descent(root, [](const binary_node<T>& n) {
            return n.last_child();
        });
    }
};

} // namespace ds
