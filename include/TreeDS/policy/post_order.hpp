#pragma once

#include <functional> // std::mem_fn

#include <TreeDS/policy/basic_policy.hpp>
#include <TreeDS/utility.hpp>

namespace md::detail {

template <typename Node, typename Allocator>
class post_order_impl final
        : public basic_policy<post_order_impl<Node, Allocator>, Node, Allocator> {

    using super = basic_policy<post_order_impl, Node, Allocator>;

    public:
    using basic_policy<post_order_impl, Node, Allocator>::basic_policy;

    Node* increment_impl() {
        if (this->current == this->root) {
            return nullptr;
        }
        Node* next_sibling = this->current->get_next_sibling();
        if (next_sibling == nullptr) {
            return this->current->get_parent();
        }
        return keep_calling(
            *next_sibling,
            [](Node& node) {
                return node.get_first_child();
            });
    }

    Node* decrement_impl() {
        Node* result = this->current->get_last_child();
        if (result) {
            return result;
        }
        return keep_calling(
            // From
            *this->current,
            // Keep calling
            [this](Node& node) {
                return &node != this->root ? node.get_parent() : nullptr;
            },
            // Until
            [this](Node& child, Node&) {
                return !child.is_first_child();
            },
            // Then return
            [](Node& child, Node&) {
                return child.get_prev_sibling();
            });
    }

    Node* go_first_impl() {
        return keep_calling(
            *this->root,
            [](Node& node) {
                return node.get_first_child();
            });
    }

    Node* go_last_impl() {
        return this->root;
    }
};

} // namespace md::detail

namespace md::policy {
struct post_order : detail::policy_tag<detail::post_order_impl> {
    // What needed is inherited.
};

} // namespace md::policy
