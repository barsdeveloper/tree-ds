#pragma once

#include <functional> // std::mem_fn()
#include <memory>     // std::allocator

#include <TreeDS/policy/basic_policy.hpp>
#include <TreeDS/utility.hpp>

namespace md::detail {

template <typename Node, typename Allocator>
class pre_order_impl final
        : public basic_policy<pre_order_impl<Node, Allocator>, Node, Allocator> {

    using super = basic_policy<pre_order_impl, Node, Allocator>;

    public:
    using basic_policy<pre_order_impl, Node, Allocator>::basic_policy;

    Node* increment_impl() {
        if (this->current->has_children()) {
            return this->current->get_first_child();
        }
        // Cross to another branch (on the right)
        Node* result = keep_calling(
            // From
            *this->current,
            // Keep calling
            [this](Node& node) {
                return &node != this->root ? node.get_parent() : nullptr;
            },
            // Until
            [&](Node& child, Node&) {
                return !child.is_last_child();
            },
            // Then return
            [&](Node& child, Node&) {
                return child.get_next_sibling();
            });
        if (result == this->root) {
            return nullptr;
        } else {
            return result;
        }
    }

    Node* decrement_impl() {
        if (this->current == this->root) {
            return nullptr;
        }
        Node* prev_sibling = this->current->get_prev_sibling();
        if (prev_sibling == nullptr) {
            return this->current->get_parent();
        }
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

    Node*
    go_last_impl() {
        return keep_calling(
            *this->root,
            [](Node& node) {
                return node.get_last_child();
            });
    }
};

} // namespace md::detail

namespace md::policy {
struct pre_order : detail::policy_tag<detail::pre_order_impl> {
    // What needed is inherited.
};

} // namespace md::policy
