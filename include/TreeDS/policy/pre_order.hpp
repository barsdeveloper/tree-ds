#pragma once

#include <functional> // std::mem_fn()
#include <memory>     // std::allocator

#include <TreeDS/policy/basic_policy.hpp>
#include <TreeDS/utility.hpp>

namespace md::detail {

template <typename Node, typename Allocator = std::allocator<Node>>
class pre_order_impl final : public basic_policy<pre_order_impl<Node, Allocator>, Node, Allocator> {

    public:
    using basic_policy<pre_order_impl, Node, Allocator>::basic_policy;

    const Node* increment_impl() {
        if (this->current->has_children()) {
            return this->current->get_first_child();
        }
        // cross to another branch (on the right)
        const Node* result = keep_calling(
            // from
            *this->current,
            // keep calling
            [&](const Node& node) {
                return node.get_parent_limit(*this->root);
            },
            // until
            [&](const Node& child, const Node&) {
                return child.get_next_sibling() != nullptr;
            },
            // then return
            [](const Node& child, const Node&) {
                return child.get_next_sibling();
            });
        if (result->is_root_limit(*this->root)) {
            return nullptr;
        } else {
            return result;
        }
    }

    const Node* decrement_impl() {
        const Node* next = this->current->get_parent_limit(*this->root);
        const Node* prev = this->current;
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

    const Node* go_first_impl() {
        return this->root;
    }

    const Node* go_last_impl() {
        return keep_calling(*this->root, std::mem_fn(&Node::get_last_child));
    }
};

} // namespace md::detail

namespace md::policy {
struct pre_order : detail::tag<detail::pre_order_impl> {
    // what needed is inherited
};
} // namespace md::policy
