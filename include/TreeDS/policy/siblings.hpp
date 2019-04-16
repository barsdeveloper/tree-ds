#pragma once

#include <functional> // std::mem_fn
#include <memory>     // std::allocator

#include <TreeDS/policy/basic_policy.hpp>
#include <TreeDS/utility.hpp>

namespace md::detail {

template <typename Node, typename Allocator>
class siblings_impl final
        : public basic_policy<siblings_impl<Node, Allocator>, Node, Allocator> {

    using super = basic_policy<siblings_impl, Node, Allocator>;

    public:
    using basic_policy<siblings_impl, Node, Allocator>::basic_policy;

    siblings_impl(Node* root, Node* current, const Allocator& allocator) :
            super(root, current, allocator) {
        this->root = this->current && this->current->get_parent()
            ? this->current->get_parent()
            : nullptr;
    }

    Node* increment_impl() {
        return this->current->get_next_sibling();
    }

    Node* decrement_impl() {
        return this->current->get_prev_sibling();
    }

    Node* go_first_impl() {
        return this->root->get_first_child();
    }

    Node* go_last_impl() {
        return this->root->get_last_child();
    }
};

} // namespace md::detail
namespace md::policy {
struct siblings : detail::policy_tag<detail::siblings_impl> {
    // What needed is inherited.
};
} // namespace md::policy
