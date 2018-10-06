#pragma once

#include <functional>

#include <TreeDS/traversal_policy/current_state_traversal.hpp>
#include <TreeDS/utility.hpp>

namespace ds {

template <typename Node>
class post_order final : public current_state_traversal<Node, post_order> {

    using current_state_traversal<Node, post_order>::current_state_traversal;

    public:
    const Node* increment_impl() {
        auto prev = this->get_current();
        auto next = this->get_current()->get_parent();
        if (!next || prev == next->get_last_child()) {
            return next;
        }
        return next->get_right_child()
            ? keep_calling(*next->get_right_child(), std::mem_fn(&Node::get_first_child))
            : nullptr;
    }

    const Node* decrement_impl() {
        using namespace std::placeholders;
        auto result = this->get_current()->get_last_child();
        if (result) {
            return result;
        }
        return keep_calling(
            // from
            *this->get_current(),
            // keep calling
            std::mem_fn(&Node::get_parent),
            // until
            [](const Node& child, const Node& parent) {
                return !child.is_first_child();
            },
            [](const Node& child, const Node& parent) {
                return child.get_prev_sibling();
            });
    }

    const Node* go_first_impl(const Node& root) {
        return keep_calling(root, std::mem_fn(&Node::get_first_child));
    }

    const Node* go_last_impl(const Node& root) {
        return &root;
    }
};

} // namespace ds
