#pragma once

#include <functional>

#include <TreeDS/utility.hpp>

namespace md {

class post_order final {

    public:
    constexpr post_order() = default;

    template <typename Node>
    const Node* increment(const Node& from) {
        const Node* prev = &from;
        const Node* next = from.get_parent();
        if (prev->is_root() || prev->is_last_child()) {
            return next;
        }
        next = prev->get_next_sibling();
        return next
            ? keep_calling(*next, std::mem_fn(&Node::get_first_child))
            : nullptr;
    }

    template <typename Node>
    const Node* decrement(const Node& from) {
        using namespace std::placeholders;
        const Node* result = from.get_last_child();
        if (result) {
            return result;
        }
        return keep_calling(
            // from
            from,
            // keep calling
            std::mem_fn(&Node::get_parent),
            // until
            [](const Node& child, const Node&) {
                return !child.is_first_child();
            },
            [](const Node& child, const Node&) {
                return child.get_prev_sibling();
            });
    }

    template <typename Node>
    const Node* go_first(const Node& root) {
        return keep_calling(root, std::mem_fn(&Node::get_first_child));
    }

    template <typename Node>
    const Node* go_last(const Node& root) {
        return &root;
    }
};

} // namespace ds
