#pragma once

#include <functional> // std::mem_fn

#include <TreeDS/policy/policy.hpp>
#include <TreeDS/utility.hpp>

namespace md {

template <typename T>
class binary_node;

namespace detail {

    template <typename Node>
    class in_order_impl {}; // unimplemented

    template <typename T>
    class in_order_impl<binary_node<T>> final : public policy<binary_node<T>> {

        public:
        in_order_impl() :
                in_order_impl(nullptr) {
        }

        in_order_impl(const binary_node<T>* root) :
                policy<binary_node<T>>(root) {
        }

        const binary_node<T>* increment(const binary_node<T>& from) {
            if (from.get_right_child()) {
                return keep_calling(
                    // from
                    *from.get_right_child(),
                    // keep calling
                    std::mem_fn(&binary_node<T>::get_left_child));
            } else {
                bool found                   = false;
                const binary_node<T>* result = keep_calling(
                    // from
                    from,
                    // keep calling
                    [&](const binary_node<T>& node) {
                        return node.get_parent_limit(*this->root);
                    },
                    // until
                    [](const binary_node<T>& child, const binary_node<T>& parent) {
                        return &child == parent.get_left_child();
                    },
                    // then return
                    [&](const binary_node<T>&, const binary_node<T>& parent) {
                        found = true;
                        return &parent;
                    });
                return found ? result : nullptr;
            }
        }

        const binary_node<T>* decrement(const binary_node<T>& from) {
            if (from.get_left_child()) {
                return keep_calling(*from.get_left_child(), std::mem_fn(&binary_node<T>::get_right_child));
            }
            return keep_calling(
                // from
                from,
                // keep calling
                [&](const binary_node<T>& node) {
                    return node.get_parent_limit(*this->root);
                },
                // until
                [](const binary_node<T>& child, const binary_node<T>& parent) {
                    return &child == parent.get_right_child();
                },
                // then return
                [](const binary_node<T>&, const binary_node<T>& parent) {
                    return &parent;
                });
        }

        const binary_node<T>* go_first(const binary_node<T>& root) {
            this->root = &root;
            return keep_calling(root, std::mem_fn(&binary_node<T>::get_left_child));
        }

        const binary_node<T>* go_last(const binary_node<T>& root) {
            this->root = &root;
            return keep_calling(root, std::mem_fn(&binary_node<T>::get_right_child));
        }
    };

} // namespace detail

struct in_order {
    template <typename Node, typename Allocator>
    detail::in_order_impl<Node> get_instance(
        const Node* root,
        const Allocator& = Allocator()) const {
        return detail::in_order_impl(root);
    }
};

} // namespace md
