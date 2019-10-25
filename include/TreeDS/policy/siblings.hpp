#pragma once

#include <TreeDS/policy/policy_base.hpp>

namespace md::detail {

template <typename NodePtr, typename NodeNavigator, typename Allocator>
class siblings_impl final
        : public policy_base<siblings_impl<NodePtr, NodeNavigator, Allocator>, NodePtr, NodeNavigator, Allocator> {

    public:
    using policy_base<siblings_impl, NodePtr, NodeNavigator, Allocator>::policy_base;

    constexpr void fix_navigator_root() {
        if (this->current && !this->navigator.get_root()) {
            NodePtr parent = this->navigator.get_parent(this->current);
            this->navigator.set_root(parent ? parent : this->current);
        }
    }

    NodePtr increment_impl() {
        return this->navigator.get_next_sibling(this->current);
    }

    NodePtr decrement_impl() {
        return this->navigator.get_prev_sibling(this->current);
    }

    NodePtr go_first_impl() {
        return this->navigator.get_first_child(this->navigator.get_root());
    }

    NodePtr go_last_impl() {
        return this->navigator.get_last_child(this->navigator.get_root());
    }
};

} // namespace md::detail
namespace md::policy {
struct siblings : detail::policy_tag<detail::siblings_impl> {
    // What needed is inherited
};
} // namespace md::policy
