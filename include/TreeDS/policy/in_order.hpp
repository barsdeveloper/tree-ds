#pragma once
#include <type_traits> // std::is_same_v

#include <TreeDS/policy/basic_policy.hpp>
#include <TreeDS/utility.hpp>

namespace md {

template <typename T>
class binary_node;

namespace detail {

    template <typename Node, typename NodeNavigator, typename Allocator, typename = void>
    class in_order_impl {
        static_assert(!std::is_same_v<Node, binary_node<std::nullptr_t>>, "In_order iteration policy is implemented only for binary_tree");
    };

    template <typename Node, typename NodeNavigator, typename Allocator>
    class in_order_impl<
        Node,
        NodeNavigator,
        Allocator,
        std::enable_if_t<is_same_template<std::decay_t<Node>, binary_node<std::nullptr_t>>>>
        final
            : public basic_policy<in_order_impl<Node, NodeNavigator, Allocator>, Node, NodeNavigator, Allocator> {

        using super = basic_policy<in_order_impl, Node, NodeNavigator, Allocator>;

        public:
        using basic_policy<in_order_impl, Node, NodeNavigator, Allocator>::basic_policy;

        Node* increment_impl() {
            Node* right = this->navigator.get_right_child(*this->current);
            if (right) {
                return keep_calling(
                    // From
                    *right,
                    // Keep calling
                    [this](Node& node) {
                        return this->navigator.get_left_child(node);
                    });
            } else {
                bool found   = false;
                Node* result = keep_calling(
                    // From
                    *this->current,
                    // Keep calling
                    [this](Node& node) {
                        return this->navigator.get_parent(node);
                    },
                    // Until
                    [this](Node& child, Node&) {
                        return this->navigator.is_left_child(child);
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
            Node* left = this->navigator.get_left_child(*this->current);
            if (left) {
                return keep_calling(
                    *left,
                    [this](Node& node) {
                        return this->navigator.get_right_child(node);
                    });
            }
            return keep_calling(
                // From
                *this->current,
                // Keep calling
                [this](Node& node) {
                    return this->navigator.get_parent(node);
                },
                // Until
                [this](Node& child, Node&) {
                    return this->navigator.is_right_child(child);
                },
                // Then return
                [](Node&, Node& parent) {
                    return &parent;
                });
        }

        Node* go_first_impl() {
            return keep_calling(
                *this->navigator.get_root(),
                [this](Node& node) {
                    return this->navigator.get_left_child(node);
                });
        }

        Node* go_last_impl() {
            return keep_calling(
                *this->navigator.get_root(),
                [this](Node& node) {
                    return this->navigator.get_right_child(node);
                });
        }
    };

} // namespace detail

namespace policy {
    struct in_order : detail::policy_tag<detail::in_order_impl> {
        // What needed is inherited
    };
} // namespace policy

} // namespace md
