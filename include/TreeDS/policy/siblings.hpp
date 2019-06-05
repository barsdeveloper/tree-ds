#pragma once

#include <TreeDS/policy/policy_base.hpp>

namespace md::detail {

template <typename Node, typename NodeNavigator, typename Allocator>
class siblings_impl final
        : public policy_base<siblings_impl<Node, NodeNavigator, Allocator>, Node, NodeNavigator, Allocator> {

    protected:
    Node* parent = nullptr;

    public:
    using policy_base<siblings_impl, Node, NodeNavigator, Allocator>::policy_base;

    Node* increment_impl() {
        return this->navigator.get_next_sibling(*this->current);
    }

    Node* decrement_impl() {
        return this->navigator.get_prev_sibling(*this->current);
    }

    Node* go_first_impl() {
        return this->navigator.get_first_child(*this->parent);
    }

    Node* go_last_impl() {
        return this->navigator.get_last_child(*this->parent);
    }
};

} // namespace md::detail
namespace md::policy {
struct siblings : detail::policy_tag<detail::siblings_impl> {
    // What needed is inherited
};
} // namespace md::policy
