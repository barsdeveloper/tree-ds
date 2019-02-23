#pragma once

#include <functional> // std::mem_fn

#include <TreeDS/policy/policy.hpp>
#include <TreeDS/utility.hpp>

namespace md {

namespace detail {

    template <typename Node>
    class post_order_impl final : public policy<Node> {

        public:
        post_order_impl() :
                post_order_impl(nullptr) {
        }

        post_order_impl(const Node* root) :
                policy<Node>(root) {
        }

        const Node* increment(const Node& from) {
            const Node* prev = &from;
            const Node* next = from.get_parent_limit(*this->root);
            if (prev->is_root_limit(*this->root) || prev->is_last_child()) {
                return next;
            }
            next = prev->get_next_sibling();
            return next
                ? keep_calling(*next, std::mem_fn(&Node::get_first_child))
                : nullptr;
        }

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
                [&](const Node& node) {
                    return node.get_parent_limit(*this->root);
                },
                // until
                [](const Node& child, const Node&) {
                    return !child.is_first_child();
                },
                [](const Node& child, const Node&) {
                    return child.get_prev_sibling();
                });
        }

        const Node* go_first(const Node& root) {
            this->root = &root;
            return keep_calling(root, std::mem_fn(&Node::get_first_child));
        }

        const Node* go_last(const Node& root) {
            this->root = &root;
            return &root;
        }
    };

} // namespace detail

struct post_order {
    template <typename Node, typename Allocator>
    detail::post_order_impl<Node> get_instance(
        const Node* root,
        const Allocator& = Allocator()) const {
        return detail::post_order_impl<Node>(root);
    }
};

} // namespace md
