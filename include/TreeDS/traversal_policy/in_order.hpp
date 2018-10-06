#pragma once

#include <functional>

#include <TreeDS/traversal_policy/current_state_traversal.hpp>
#include <TreeDS/utility.hpp>

namespace ds {

template <typename T>
class binary_node;

template <typename Node>
class in_order final {};

template <typename T>
class in_order<binary_node<T>> final : public current_state_traversal<binary_node<T>, in_order> {

    using current_state_traversal<binary_node<T>, in_order>::current_state_traversal;

    public:
    const binary_node<T>* increment_impl() {
        const binary_node<T>* current = this->get_current();
        if (current->get_right_child()) {
            return keep_calling(*current->get_right_child(), std::mem_fn(&binary_node<T>::get_left_child));
        } else {
            auto prev = current;
            auto next = current->get_parent();
            while (next) {
                if (prev == next->get_left_child()) {
                    return next; // found
                }
                prev = next;
                next = next->get_parent();
            }
            return next;
        }
    }

    const binary_node<T>* decrement_impl() {
        const binary_node<T>* current = this->get_current();
        if (current->get_left_child()) {
            return keep_calling(*current->get_left_child(), std::mem_fn(&binary_node<T>::get_right_child));
        }
        auto prev = current;
        auto next = current->get_parent();
        while (next) {
            if (prev == next->get_right_child()) {
                return next; // found
            }
            prev = next;
            next = next->get_parent();
        }
        return next;
    }

    const binary_node<T>* go_first_impl(const binary_node<T>& root) {
        return keep_calling(root, std::mem_fn(&binary_node<T>::get_left_child));
    }

    const binary_node<T>* go_last_impl(const binary_node<T>& root) {
        return keep_calling(root, std::mem_fn(&binary_node<T>::get_right_child));
    }
};

} // namespace ds
