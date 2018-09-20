#pragma once

#include <functional>

#include <TreeDS/utility.hpp>

namespace ds {

template <typename>
class binary_node;

template <typename>
class nary_node;

class pre_order final {

    public:
    constexpr pre_order() = default;

    template <typename Node>
    const Node* increment(const Node& from) const {
        auto result = from.get_first_child();
        if (result) {
            return result;
        } else {
            return cross_bridge_right(from);
        }
    }

    template <typename Node>
    const Node* decrement(const Node& node) const {
        const Node* next = node.get_parent();
        const Node* prev = &node;
        /*
         * The parent is the next node if (REMEMBER: we traverse tree in pre-order and decrement the iterator):
         *   1) The passed node is root (its parent is nullptr so the previous value is the end of the reverse iterator)
         *   2) The node is the unique child of its parent
         */
        if (!next || prev == next->get_first_child()) {
            return next;
        }
        const Node* result = descent(
            prev->get_prev_sibling(),          // from
            std::mem_fn(&Node::get_last_child) // keep calling
        );
        return result;
    }

    template <typename Node>
    const Node* go_first(const Node& root) const {
        return &root;
    }

    template <typename Node>
    const Node* go_last(const Node& root) const {
        return descent(&root, std::mem_fn(&Node::get_last_child));
    }
};

} // namespace ds
