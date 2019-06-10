#pragma once

#include <TreeDS/policy/policy_base.hpp>
#include <TreeDS/utility.hpp>

namespace md::detail {

template <typename NodePtr, typename NodeNavigator, typename Allocator>
class post_order_impl final
        : public policy_base<post_order_impl<NodePtr, NodeNavigator, Allocator>, NodePtr, NodeNavigator, Allocator> {

    public:
    using policy_base<post_order_impl, NodePtr, NodeNavigator, Allocator>::policy_base;

    NodePtr increment_impl() {
        if (this->navigator.is_root(this->current)) {
            return nullptr;
        }
        NodePtr next_sibling = this->navigator.get_next_sibling(this->current);
        if (next_sibling == nullptr) {
            return this->navigator.get_parent(this->current);
        }
        return keep_calling(
            next_sibling,
            [this](NodePtr node) {
                return this->navigator.get_first_child(node);
            });
    }

    NodePtr decrement_impl() {
        NodePtr result = this->navigator.get_last_child(this->current);
        if (result) {
            return result;
        }
        return keep_calling(
            // From
            this->current,
            // Keep calling
            [this](NodePtr node) {
                return this->navigator.get_parent(node);
            },
            // Until
            [this](NodePtr child, NodePtr) {
                return !this->navigator.is_first_child(child);
            },
            // Then return
            [this](NodePtr child, NodePtr) {
                return this->navigator.get_prev_sibling(child);
            });
    }

    NodePtr go_first_impl() {
        return this->navigator.get_highest_left_leaf();
    }

    NodePtr go_last_impl() {
        return this->navigator.get_root();
    }
};

} // namespace md::detail

namespace md::policy {
struct post_order : detail::policy_tag<detail::post_order_impl> {
    // What needed is inherited
};

} // namespace md::policy
