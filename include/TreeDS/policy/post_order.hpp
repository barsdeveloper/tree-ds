#pragma once

#include <functional> // std::mem_fn

#include <TreeDS/policy/basic_policy.hpp>
#include <TreeDS/utility.hpp>

namespace md::detail {

template <typename Node, typename NodeNavigator, typename Allocator>
class post_order_impl final
        : public basic_policy<post_order_impl<Node, NodeNavigator, Allocator>, Node, NodeNavigator, Allocator> {

    using super = basic_policy<post_order_impl, Node, NodeNavigator, Allocator>;

    public:
    using basic_policy<post_order_impl, Node, NodeNavigator, Allocator>::basic_policy;

    Node* increment_impl() {
        if (this->navigator.is_root(*this->current)) {
            return nullptr;
        }
        Node* next_sibling = this->navigator.get_next_sibling(*this->current);
        if (next_sibling == nullptr) {
            return this->navigator.get_parent(*this->current);
        }
        return keep_calling(
            *next_sibling,
            [this](Node& node) {
                return this->navigator.get_first_child(node);
            });
    }

    Node* decrement_impl() {
        Node* result = this->navigator.get_last_child(*this->current);
        if (result) {
            return result;
        }
        return keep_calling(
            // From
            *this->current,
            // Keep calling
            [this](Node& node) {
                return this->navigator.get_parent(node);
            },
            // Until
            [this](Node& child, Node&) {
                return !this->navigator.is_first_child(child);
            },
            // Then return
            [this](Node& child, Node&) {
                return this->navigator.get_prev_sibling(child);
            });
    }

    Node* go_first_impl() {
        return this->navigator.get_highest_left_leaf();
    }

    Node* go_last_impl() {
        return this->navigator.get_root();
    }
};

} // namespace md::detail

namespace md::policy {
struct post_order : detail::policy_tag<detail::post_order_impl> {
    // What needed is inherited.
};

} // namespace md::policy
