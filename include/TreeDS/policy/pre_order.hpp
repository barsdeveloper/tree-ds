#pragma once

#include <TreeDS/policy/policy_base.hpp>
#include <TreeDS/utility.hpp>

namespace md::detail {

template <typename Node, typename NodeNavigator, typename Allocator>
class pre_order_impl final
        : public policy_base<pre_order_impl<Node, NodeNavigator, Allocator>, Node, NodeNavigator, Allocator> {

    public:
    using policy_base<pre_order_impl, Node, NodeNavigator, Allocator>::policy_base;

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

    // Used by any_matcher, please ignore
    /**
     * @brief Go in depth by traversing first children until a ramification is found.
     * @return the first child {@b after} of the ramification's node
     */
    Node* go_depth_first_ramification() {
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
