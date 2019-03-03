#pragma once

#include <functional>  // std::mem_fn()
#include <memory>      // std::allocator
#include <type_traits> // std::is_same_v

#include <TreeDS/policy/basic_policy.hpp>
#include <TreeDS/utility.hpp>

namespace md {

template <typename T>
class binary_node;

namespace detail {

    template <typename Node, typename Allocator = std::allocator<Node>>
    class in_order_impl {
        static_assert(!std::is_same_v<Node, binary_node<int>>, "In_order iteration policy is implemented only for binary_tree");
    };

    template <typename T, typename Allocator>
    class in_order_impl<binary_node<T>, Allocator> final
            : public basic_policy<
                  in_order_impl<binary_node<T>, Allocator>,
                  binary_node<T>,
                  Allocator> {

        public:
        using basic_policy<in_order_impl, binary_node<T>, Allocator>::basic_policy;

        const binary_node<T>* increment_impl() {
            if (this->current->get_right_child()) {
                return keep_calling(
                    // from
                    *this->current->get_right_child(),
                    // keep calling
                    std::mem_fn(&binary_node<T>::get_left_child));
            } else {
                bool found                   = false;
                const binary_node<T>* result = keep_calling(
                    // from
                    *this->current,
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

        const binary_node<T>* decrement_impl() {
            if (this->current->get_left_child()) {
                return keep_calling(*this->current->get_left_child(), std::mem_fn(&binary_node<T>::get_right_child));
            }
            return keep_calling(
                // from
                *this->current,
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

        const binary_node<T>* go_first_impl() {
            return keep_calling(*this->root, std::mem_fn(&binary_node<T>::get_left_child));
        }

        const binary_node<T>* go_last_impl() {
            return keep_calling(*this->root, std::mem_fn(&binary_node<T>::get_right_child));
        }
    };

} // namespace detail

namespace policy {
    struct in_order : detail::tag<detail::in_order_impl> {
        // what needed is inherited
    };
} // namespace policy

} // namespace md
