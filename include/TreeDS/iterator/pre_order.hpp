#pragma once

#include <functional>

#include <TreeDS/iterator/helper.hpp>

namespace ds {

template <typename>
class binary_node;

template <typename, typename>
class node;

class pre_order final {

    public:
    constexpr pre_order() = default;

    template <typename T>
    const binary_node<T>* increment(const binary_node<T>& from) const {
        auto result = from.first_child();
        if (result) {
            return result;
        } else {
            return cross_bridge_right(from);
        }
    }

    template <typename T>
    const binary_node<T>* decrement(const binary_node<T>& node) const {
        const binary_node<T>* next = node.get_parent();
        const binary_node<T>* prev = &node;
        /*
		 * The parent is the next node if (REMEMBER: we traverse tree in pre-order and decrement the iterator):
		 *     1) The passed node is root (its parent is nullptr so the previous value is the end of the reverse iterator).
		 *     2) The node is the unique child of its parent
		 */
        if (!next || prev == next->first_child()) {
            return next;
        }
        return descent(next->get_left(), std::mem_fn(&binary_node<T>::last_child));
    }

    template <typename T>
    const binary_node<T>* go_first(const binary_node<T>& root) const {
        return &root;
    }

    template <typename T>
    const binary_node<T>* go_last(const binary_node<T>& root) const {
        return descent(&root, std::mem_fn(&binary_node<T>::last_child));
    }
};

} // namespace ds
