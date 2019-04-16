#pragma once

#include <memory>      // std::allocator
#include <type_traits> // std::is_same_v

#include <TreeDS/policy/basic_policy.hpp>
#include <TreeDS/utility.hpp>

namespace md {

template <typename T>
class binary_node;

namespace detail {

    template <typename Node, typename Allocator, typename = void>
    class in_order_impl {
        static_assert(!std::is_same_v<Node, binary_node<std::nullptr_t>>, "In_order iteration policy is implemented only for binary_tree");
    };

    template <typename Node, typename Allocator>
    class in_order_impl<
        Node,
        Allocator,
        std::enable_if_t<is_same_template<std::decay_t<Node>, binary_node<std::nullptr_t>>>>
        final
            : public basic_policy<in_order_impl<Node, Allocator>, Node, Allocator> {

        using super = basic_policy<in_order_impl, Node, Allocator>;

        public:
        using basic_policy<in_order_impl, Node, Allocator>::basic_policy;

        Node* increment_impl() {
            if (this->current->has_right_child()) {
                return keep_calling(
                    // From
                    *this->current->get_right_child(),
                    // Keep calling
                    [](Node& node) {
                        return node.get_left_child();
                    });
            } else {
                bool found   = false;
                Node* result = keep_calling(
                    // From
                    *this->current,
                    // Keep calling
                    [this](Node& node) {
                        return &node != this->root ? node.get_parent() : nullptr;
                    },
                    // Until
                    [](Node& child, Node&) {
                        return child.is_left_child();
                    },
                    // Then return
                    [&](Node&, Node& parent) {
                        found = true;
                        return &parent;
                    });
                return found ? result : nullptr;
            }
        }

        Node* decrement_impl() {
            if (this->current->has_left_child()) {
                return keep_calling(
                    *this->current->get_left_child(),
                    [](Node& node) {
                        return node.get_right_child();
                    });
            }
            return keep_calling(
                // From
                *this->current,
                // Keep calling
                [this](Node& node) {
                    return &node != this->root ? node.get_parent() : nullptr;
                },
                // Until
                [](Node& child, Node&) {
                    return child.is_right_child();
                },
                // Then return
                [](Node&, Node& parent) {
                    return &parent;
                });
        }

        Node* go_first_impl() {
            return keep_calling(
                *this->root,
                [](Node& node) {
                    return node.get_left_child();
                });
        }

        Node* go_last_impl() {
            return keep_calling(
                *this->root,
                [](Node& node) {
                    return node.get_right_child();
                });
        }
    };

} // namespace detail

namespace policy {
    struct in_order : detail::policy_tag<detail::in_order_impl> {
        // What needed is inherited.
    };
} // namespace policy

} // namespace md
