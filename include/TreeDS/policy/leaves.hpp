#pragma once

#include <stack>

#include <TreeDS/policy/policy_base.hpp>

namespace md::detail {

template <typename NodePtr, typename NodeNavigator, typename Allocator>
class leaves_impl final
        : public policy_base<leaves_impl<NodePtr, NodeNavigator, Allocator>, NodePtr, NodeNavigator, Allocator> {

    protected:
    std::stack<NodePtr> ancestors {};

    public:
    using policy_base<leaves_impl, NodePtr, NodeNavigator, Allocator>::policy_base;

    NodePtr increment_impl() {
        if (this->ancestors.empty()) {
            return nullptr;
        }
        NodePtr node(this->navigator.get_next_sibling(this->ancestors.top()));
        this->ancestors.pop();
        return keep_calling(
            node,
            [this](NodePtr& node) {
                if (this->navigator.get_next_sibling(node)) {
                    this->ancestors.push(node);
                }
                return this->navigator.get_first_child(node);
            });
    }

    NodePtr decrement_impl() {
        // Go up to cross a branch on left
        NodePtr node(keep_calling(
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
            }));
        if (!node) {
            return node;
        }
        this->ancestors.push(node);
        return keep_calling(
            node,
            [this](NodePtr& node) {
                return this->navigator.get_last_child(node);
            });
    }

    NodePtr go_first_impl() {
        return keep_calling(
            this->navigator.get_root(),
            [this](NodePtr& node) {
                if (this->navigator.get_next_sibling(node)) {
                    this->ancestors.push(node);
                }
                return this->navigator.get_first_child(node);
            });
    }

    NodePtr go_last_impl() {
        this->ancestors = std::stack<NodePtr>();
        return keep_calling(
            this->navigator.get_root(),
            [this](NodePtr& node) {
                return this->navigator.get_last_child(node);
            });
    }
};

} // namespace md::detail
namespace md::policy {
struct leaves : detail::policy_tag<detail::leaves_impl> {
    // What needed is inherited
};
} // namespace md::policy
