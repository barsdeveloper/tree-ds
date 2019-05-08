#pragma once

#include <functional> // std::mem_fn
#include <memory>     // std::allocator

#include <TreeDS/policy/basic_policy.hpp>
#include <TreeDS/utility.hpp>

namespace md::detail {

template <typename Node, typename NodeNavigator, typename Allocator>
class siblings_impl final
        : public basic_policy<siblings_impl<Node, NodeNavigator, Allocator>, Node, NodeNavigator, Allocator> {

    using super = basic_policy<siblings_impl, Node, NodeNavigator, Allocator>;

    protected:
    Node* parent = nullptr;

    public:
    using basic_policy<siblings_impl, Node, NodeNavigator, Allocator>::basic_policy;

    siblings_impl(Node* root, Node* current, const Allocator& allocator) :
            super(root, current, allocator),
            parent(this->current ? this->navigator.get_parent(*this->current) : nullptr) {
    }

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
    // What needed is inherited.
};
} // namespace md::policy
