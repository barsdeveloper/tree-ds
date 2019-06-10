#pragma once
#include <type_traits> // std::is_same_v

#include <TreeDS/policy/policy_base.hpp>
#include <TreeDS/utility.hpp>

namespace md {

template <typename T>
class binary_node;

namespace detail {

    template <typename NodePtr, typename NodeNavigator, typename Allocator, typename = void>
    class in_order_impl {
        // Empty
    };

    template <typename NodePtr, typename NodeNavigator, typename Allocator>
    class in_order_impl<
        NodePtr,
        NodeNavigator,
        Allocator,
        std::enable_if_t<is_same_template<std::decay_t<std::remove_pointer_t<NodePtr>>, binary_node<std::nullptr_t>>>>
        final
            : public policy_base<in_order_impl<NodePtr, NodeNavigator, Allocator>, NodePtr, NodeNavigator, Allocator> {

        public:
        using policy_base<in_order_impl, NodePtr, NodeNavigator, Allocator>::policy_base;

        NodePtr increment_impl() {
            NodePtr right = this->navigator.get_right_child(this->current);
            if (right) {
                return keep_calling(
                    // From
                    right,
                    // Keep calling
                    [this](NodePtr node) {
                        return this->navigator.get_left_child(node);
                    });
            } else {
                bool found     = false;
                NodePtr result = keep_calling(
                    // From
                    this->current,
                    // Keep calling
                    [this](NodePtr node) {
                        return this->navigator.get_parent(node);
                    },
                    // Until
                    [this](NodePtr child, NodePtr) {
                        return this->navigator.is_left_child(child);
                    },
                    // Then return
                    [&](NodePtr, NodePtr parent) {
                        found = true;
                        return parent;
                    });
                return found ? result : nullptr;
            }
        }

        NodePtr decrement_impl() {
            NodePtr left = this->navigator.get_left_child(this->current);
            if (left) {
                return keep_calling(
                    left,
                    [this](NodePtr node) {
                        return this->navigator.get_right_child(node);
                    });
            }
            return keep_calling(
                // From
                this->current,
                // Keep calling
                [this](NodePtr node) {
                    return this->navigator.get_parent(node);
                },
                // Until
                [this](NodePtr child, NodePtr) {
                    return this->navigator.is_right_child(child);
                },
                // Then return
                [](NodePtr, NodePtr parent) {
                    return parent;
                });
        }

        NodePtr go_first_impl() {
            return keep_calling(
                this->navigator.get_root(),
                [this](NodePtr node) {
                    return this->navigator.get_left_child(node);
                });
        }

        NodePtr go_last_impl() {
            return keep_calling(
                this->navigator.get_root(),
                [this](NodePtr node) {
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
