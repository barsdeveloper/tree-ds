#pragma once

#include <functional> // std::mem_fn()
#include <memory>     // std::allocator

#include <TreeDS/policy/basic_policy.hpp>
#include <TreeDS/utility.hpp>

namespace md::detail {

template <typename Node, typename Allocator>
class pre_order_impl final : public basic_policy<pre_order_impl<Node, Allocator>, Node, Allocator> {

    public:
    using basic_policy<pre_order_impl, Node, Allocator>::basic_policy;

    Node* increment_impl() {
        if (this->current->has_children()) {
            return this->current->get_first_child();
        }
        // cross to another branch (on the right)
        Node* result = keep_calling(
            // from
            *this->current,
            // keep calling
            [&](Node& node) {
                return node.get_parent_limit(*this->root);
            },
            // until
            [&](Node& child, Node&) {
                return child.get_next_sibling() != nullptr;
            },
            // then return
            [](Node& child, Node&) {
                return child.get_next_sibling();
            });
        if (result->is_root_limit(*this->root)) {
            return nullptr;
        } else {
            return result;
        }
    }

    Node* decrement_impl() {
        Node* next = this->current->get_parent_limit(*this->root);
        Node* prev = this->current;
        /*
             * The parent is the next node if (REMEMBER: we traverse tree in pre-order and decrement the iterator):
             *   1) The passed node is root (its parent is nullptr so the previous value is the end of the reverse iterator)
             *   2) The node is the unique child of its parent
             */
        if (!next || prev == next->get_first_child()) {
            return next;
        }
        Node* prev_sibling = prev->get_prev_sibling();
        return prev_sibling
            ? keep_calling(
                  *prev_sibling,
                  [](Node& node) {
                      return node.get_last_child();
                  })
            : nullptr;
    }

    Node* go_first_impl() {
        return this->root;
    }

    Node* go_last_impl() {
        return keep_calling(
            *this->root,
            [](Node& node) {
                return node.get_last_child();
            });
    }
};

} // namespace md::detail

namespace md::policy {
struct pre_order : detail::tag<detail::pre_order_impl> {
    // what needed is inherited
};
} // namespace md::policy
