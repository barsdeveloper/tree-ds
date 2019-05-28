#pragma once

#include <TreeDS/policy/basic_policy.hpp>

namespace md::detail {

template <typename Node, typename NodeNavigator, typename Allocator>
class fixed_impl final
        : public basic_policy<fixed_impl<Node, NodeNavigator, Allocator>, Node, NodeNavigator, Allocator> {

    protected:
    Node* parent = nullptr;

    public:
    using basic_policy<fixed_impl, Node, NodeNavigator, Allocator>::basic_policy;

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
struct fixed : detail::policy_tag<detail::fixed_impl> {
    // What needed is inherited
};
} // namespace md::policy
