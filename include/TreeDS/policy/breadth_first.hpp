#pragma once

#include <deque>
#include <memory> // std::allocator

#include <TreeDS/node/node.hpp>
#include <TreeDS/node/node_navigator.hpp>
#include <TreeDS/policy/basic_policy.hpp>

namespace md::detail {

/**
 * Traversal policy that returns nodes in a line by line fashion. In forward order the nodes will be retrieved from left
 * to right, and from top to bottom.  Please note that this iterator is intended to be usedforward only (incremented
 * only). Reverse order iteration is possible (and tested) though it will imply severe performance drop.
 */
template <typename Node, typename NodeNavigator, typename Allocator>
class breadth_first_impl final
        : public basic_policy<breadth_first_impl<Node, NodeNavigator, Allocator>, Node, NodeNavigator, Allocator> {

    using super          = basic_policy<breadth_first_impl, Node, NodeNavigator, Allocator>;
    using allocator_type = typename std::allocator_traits<Allocator>::template rebind_alloc<std::decay_t<Node>>;
    using typename super::node_type;

    private:
    std::deque<node_type*, Allocator> open_nodes = manage_initial_status();
    allocator_type allocator;

    public:
    using basic_policy<breadth_first_impl, Node, NodeNavigator, Allocator>::basic_policy;

    // Formward puhes into open back and pops front
    Node* increment_impl() {
        if (this->open_nodes.empty()) {
            return nullptr;
        }
        // Get element to be returned
        Node* result = this->open_nodes.front();
        // Manage next sibling replacement in queue
        Node* sibling = this->navigator.get_next_sibling(*result);
        if (sibling) {
            this->open_nodes.front() = sibling;
        } else {
            this->open_nodes.pop_front();
        }
        // Push back its first child
        Node* first_child = this->navigator.get_first_child(*result);
        if (first_child) {
            this->open_nodes.push_back(first_child);
        }
        return result;
    }

    Node* decrement_impl() {
        // Delete the child of current node from open_nodes
        Node* first_child = this->navigator.get_first_child(*this->current);
        if (first_child) {
            assert(this->navigator.get_parent(*this->open_nodes.back()) == this->current);
            // Delete the child of the previous node from open_nodes (invariants garantee that it is the last element)
            this->open_nodes.pop_back();
        }
        // Delete next sibling of the current node from open_nodes
        if (this->navigator.get_next_sibling(*this->current)) {
            assert(this->navigator.get_prev_sibling(*this->open_nodes.front()) == this->current);
            this->open_nodes.pop_front();
        }
        // Calculate the previous element
        Node* result = this->navigator.get_left_branch(*this->current);
        if (result == nullptr) {
            result = this->navigator.get_same_row_rightmost(*this->navigator.get_parent(*this->current));
        }
        // Update queue
        this->open_nodes.push_front(this->current);
        return result;
    }

    Node* go_first_impl() {
        this->open_nodes.clear();
        this->open_nodes.push_back(this->navigator.get_root());
        return this->increment_impl();
    }

    Node* go_last_impl() {
        this->open_nodes.clear();
        return this->navigator.get_deepest_rightmost_leaf();
    }

    std::deque<Node*, Allocator> manage_initial_status() {
        std::deque<Node*, Allocator> result;
        if (this->current == nullptr) {
            return result;
        }
        Node* node;
        auto process_child = [&]() {
            Node* first_child = this->navigator.get_first_child(*node);
            if (first_child) {
                result.push_back(first_child);
            }
        };
        // Manage next_sibling insertion
        Node* next = this->navigator.get_next_sibling(*this->current);
        if (next) {
            result.push_back(next);
        }
        // Manage right elements
        node = this->navigator.is_root(*this->current)
            ? nullptr
            : this->navigator.get_right_branch(*this->current->get_parent());
        while (node) {
            process_child();
            node = this->navigator.get_right_branch(*node);
        }
        // Manage lower row, left elements
        node = this->navigator.get_same_row_leftmost(*this->current);
        while (node != nullptr && node != this->current) {
            process_child();
            node = this->navigator.get_right_branch(*node);
        }
        // Manage current node child
        node = this->current;
        process_child();
        return std::move(result);
    }

    void update(Node& current, Node* replacement) {
        // Delete child of the previous nodes from open_nodes
        if (this->navigator.get_first_child(current)) {
            assert(this->navigator.get_parent(*this->open_nodes.back()) == &current);
            this->open_nodes.pop_back();
        }
        // Push from back the children of the replacement node
        Node* child = replacement != nullptr
            ? this->navigator.get_first_child(*replacement)
            : nullptr;
        if (child != nullptr) {
            this->open_nodes.push_back(child);
        }
        this->super::update(current, replacement);
    }
};

} // namespace md::detail

namespace md::policy {

struct breadth_first : detail::policy_tag<detail::breadth_first_impl> {
    // What needed is inherited
};
} // namespace md::policy
