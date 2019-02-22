#pragma once

#include <functional>

#include <TreeDS/policy/policy.hpp>
#include <TreeDS/utility.hpp>

namespace md {

namespace detail {

    template <typename Node>
    class pre_order_impl final : public policy<Node> {

        public:
        pre_order_impl() :
                pre_order_impl(nullptr) {
        }

        pre_order_impl(const Node* root) :
                policy<Node>(root) {
        }

        const Node* increment(const Node& from) {
            const Node* result = from.get_first_child();
            if (result != nullptr) {
                return result;
            }
            // cross to another branch (on the right)
            result = keep_calling(
                // from
                from,
                // keep calling
                [&](const Node& node) {
                    return node.get_parent_limit(*this->root);
                },
                // until
                [&](const Node& child, const Node&) {
                    return child.get_next_sibling() != nullptr;
                },
                // then return
                [](const Node& child, const Node&) {
                    return child.get_next_sibling();
                });
            if (result->is_root_limit(*this->root)) {
                return nullptr;
            } else {
                return result;
            }
        }

        const Node* decrement(const Node& from) {
            const Node* next = from.get_parent_limit(*this->root);
            const Node* prev = &from;
            /*
             * The parent is the next node if (REMEMBER: we traverse tree in pre-order and decrement the iterator):
             *   1) The passed node is root (its parent is nullptr so the previous value is the end of the reverse iterator)
             *   2) The node is the unique child of its parent
             */
            if (!next || prev == next->get_first_child()) {
                return next;
            }
            const Node* prev_sibling = prev->get_prev_sibling();
            return prev_sibling
                ? keep_calling(*prev_sibling, std::mem_fn(&Node::get_last_child))
                : nullptr;
        }

        const Node* go_first(const Node& root) {
            this->root = &root;
            return &root;
        }

        const Node* go_last(const Node& root) {
            this->root = &root;
            return keep_calling(root, std::mem_fn(&Node::get_last_child));
        }
    };

} // namespace detail

struct pre_order {
    template <typename Node, typename Allocator>
    detail::pre_order_impl<Node> get_instance(
        const Node* root,
        const Allocator& = Allocator()) const {
        return detail::pre_order_impl<Node>(root);
    }
};

} // namespace md
