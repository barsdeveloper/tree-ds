#pragma once

#include <TreeDS/policy/policy_base.hpp>
#include <TreeDS/utility.hpp>

namespace md::detail {

template <typename NodePtr, typename NodeNavigator, typename Allocator>
class pre_order_impl final
        : public policy_base<pre_order_impl<NodePtr, NodeNavigator, Allocator>, NodePtr, NodeNavigator, Allocator> {

    public:
    using policy_base<pre_order_impl, NodePtr, NodeNavigator, Allocator>::policy_base;

    NodePtr increment_impl() {
        NodePtr first_child = this->navigator.get_first_child(this->current);
        if (first_child) {
            return first_child;
        }
        // Cross to another branch (on the right)
        NodePtr result = keep_calling(
            // From
            this->current,
            // Keep calling
            [this](NodePtr node) {
                return this->navigator.get_parent(node);
            },
            // Until
            [this](NodePtr child, NodePtr) {
                return !this->navigator.is_last_child(child);
            },
            // Then return
            [&](NodePtr child, NodePtr) {
                return this->navigator.get_next_sibling(child);
            });
        return this->navigator.is_root(result) ? nullptr : result;
    }

    NodePtr decrement_impl() {
        if (this->navigator.is_root(this->current)) {
            return nullptr;
        }
        NodePtr prev_sibling = this->navigator.get_prev_sibling(this->current);
        if (prev_sibling == nullptr) {
            return this->navigator.get_parent(this->current);
        }
        return prev_sibling
            ? keep_calling(
                prev_sibling,
                [this](NodePtr node) {
                    return this->navigator.get_last_child(node);
                })
            : nullptr;
    }

    NodePtr go_first_impl() {
        return this->navigator.get_root();
    }

    NodePtr go_last_impl() {
        return this->navigator.get_highest_right_leaf();
    }

    // Used by multi_matcher, please ignore
    /**
     * @brief Go in depth by traversing first children until a ramification is found.
     * @return the first child {@b after} of the ramification's node
     */
    pre_order_impl& go_depth_first_ramification() {
        assert(this->current);
        bool found     = false;
        NodePtr result = keep_calling(
            // From
            this->current,
            // Keep calling
            [this](NodePtr node) {
                return this->navigator.get_first_child(node);
            },
            // Until
            [&](NodePtr, NodePtr child) {
                found = true;
                return !this->navigator.is_last_child(child);
            },
            // Then return
            [](NodePtr, NodePtr child) {
                return child;
            });
        this->current = found ? result : nullptr;
        return *this;
    }
};

} // namespace md::detail

namespace md::policy {
struct pre_order : detail::policy_tag<detail::pre_order_impl> {
    // What needed is inherited
};

} // namespace md::policy
