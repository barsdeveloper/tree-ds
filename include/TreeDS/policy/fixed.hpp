#pragma once

#include <TreeDS/policy/policy_base.hpp>

namespace md::detail {

template <typename NodePtr, typename NodeNavigator, typename Allocator>
class fixed_impl final
        : public policy_base<fixed_impl<NodePtr, NodeNavigator, Allocator>, NodePtr, NodeNavigator, Allocator> {

    protected:
    NodePtr initial = this->current;

    public:
    using policy_base<fixed_impl, NodePtr, NodeNavigator, Allocator>::policy_base;

    NodePtr increment_impl() {
        return NodePtr();
    }

    NodePtr decrement_impl() {
        return NodePtr();
    }

    NodePtr go_first_impl() {
        return this->initial;
    }

    NodePtr go_last_impl() {
        return this->initial;
    }
};

} // namespace md::detail
namespace md::policy {
struct fixed : detail::policy_tag<detail::fixed_impl> {
    // What needed is inherited
};
} // namespace md::policy
