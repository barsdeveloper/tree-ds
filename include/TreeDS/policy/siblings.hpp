#pragma once

#include <functional> // std::mem_fn
#include <memory>     // std::allocator

#include <TreeDS/policy/basic_policy.hpp>
#include <TreeDS/utility.hpp>

namespace md::detail {

template <typename Node, typename Allocator = std::allocator<Node>>
class siblings_impl final : public basic_policy<siblings_impl<Node, Allocator>, Node, Allocator> {

    public:
    using basic_policy<siblings_impl, Node, Allocator>::basic_policy;

    siblings_impl(const Node* root, const Node* current, const Allocator& allocator) :
            basic_policy<siblings_impl, Node, Allocator>(root, current, allocator) {
        this->root = this->current && this->current->get_parent()
            ? this->current->get_parent()
            : nullptr;
    }

    const Node* increment_impl() {
        return this->current->get_next_sibling();
    }

    const Node* decrement_impl() {
        return this->current->get_prev_sibling();
    }

    const Node* go_first_impl() {
        return this->root->get_first_child();
    }

    const Node* go_last_impl() {
        return this->root->get_last_child();
    }
};

} // namespace md::detail
namespace md::policy {
struct siblings : detail::tag<detail::siblings_impl> {
    // what needed is inherited
};
} // namespace md::policy
