#pragma once

#include <functional>

#include <TreeDS/traversal_policy/current_state_traversal.hpp>
#include <TreeDS/utility.hpp>

namespace ds {

template <typename Node>
class pre_order final : public current_state_traversal<Node, pre_order> {

    using current_state_traversal<Node, pre_order>::current_state_traversal;

    public:
    const Node* increment_impl() {
        auto result = this->get_current()->get_first_child();
        if (result != nullptr) {
            return result;
        }
        // cross to another branch (on the right)
        result = keep_calling(
            // from
            *this->get_current(),
            // keep calling
            std::mem_fn(&Node::get_parent),
            // until
            [](const Node& child, const Node&) {
                return child.get_next_sibling() != nullptr;
            },
            // then return
            [](const Node& child, const Node&) {
                return child.get_next_sibling();
            });
        if (result->is_root()) {
            return nullptr;
        } else {
            return result;
        }
    }

    const Node* decrement_impl() {
        const Node* next = this->get_current()->get_parent();
        const Node* prev = this->get_current();
        /*
         * The parent is the next node if (REMEMBER: we traverse tree in pre-order and decrement the iterator):
         *   1) The passed node is root (its parent is nullptr so the previous value is the end of the reverse iterator)
         *   2) The node is the unique child of its parent
         */
        if (!next || prev == next->get_first_child()) {
            return next;
        }
        const Node* prev_sibling = prev->get_prev_sibling();
        return prev_sibling
            ? keep_calling(*prev_sibling, std::mem_fn(&Node::get_last_child))
            : nullptr;
    }

    const Node* go_first_impl(const Node& root) {
        return &root;
    }

    const Node* go_last_impl(const Node& root) {
        return keep_calling(root, std::mem_fn(&Node::get_last_child));
    }
};

} // namespace ds
