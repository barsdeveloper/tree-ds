#pragma once

#include <TreeDS/policy/basic_policy.hpp>
#include <TreeDS/utility.hpp>

namespace md::detail {

template <typename Node, typename NodeNavigator, typename Allocator>
class pre_order_impl final
        : public basic_policy<pre_order_impl<Node, NodeNavigator, Allocator>, Node, NodeNavigator, Allocator> {

    using super = basic_policy<pre_order_impl, Node, NodeNavigator, Allocator>;

    public:
    using basic_policy<pre_order_impl, Node, NodeNavigator, Allocator>::basic_policy;

    Node* increment_impl() {
        Node* first_child = this->navigator.get_first_child(*this->current);
        if (first_child) {
            return first_child;
        }
        // Cross to another branch (on the right)
        Node* result = keep_calling(
            // From
            *this->current,
            // Keep calling
            [this](Node& node) {
                return this->navigator.get_parent(node);
            },
            // Until
            [this](Node& child, Node&) {
                return !this->navigator.is_last_child(child);
            },
            // Then return
            [&](Node& child, Node&) {
                return this->navigator.get_next_sibling(child);
            });
        return this->navigator.is_root(*result) ? nullptr : result;
    }

    Node* decrement_impl() {
        if (this->navigator.is_root(*this->current)) {
            return nullptr;
        }
        Node* prev_sibling = this->navigator.get_prev_sibling(*this->current);
        if (prev_sibling == nullptr) {
            return this->navigator.get_parent(*this->current);
        }
        return prev_sibling
            ? keep_calling(
                  *prev_sibling,
                  [this](Node& node) {
                      return this->navigator.get_last_child(node);
                  })
            : nullptr;
    }

    Node* go_first_impl() {
        return this->navigator.get_root();
    }

    Node* go_last_impl() {
        return this->navigator.get_highest_right_leaf();
    }

    Node* depth_increment() {
        bool found   = false;
        Node* result = keep_calling(
            // From
            *this->current,
            // Keep calling
            [this](Node& node) {
                return this->navigator.get_first_child(node);
            },
            // Until
            [&](Node&, Node& child) {
                found = true;
                return !this->navigator.is_last_child(child);
            },
            // Then return
            [this](Node&, Node& child) {
                return &child;
            });
        return found ? result : nullptr;
    }
};

} // namespace md::detail

namespace md::policy {
struct pre_order : detail::policy_tag<detail::pre_order_impl> {
    // What needed is inherited
};

} // namespace md::policy
