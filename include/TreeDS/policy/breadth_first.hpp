#pragma once

#include <deque>
#include <memory> // std::allocator

#include <TreeDS/node/node.hpp>
#include <TreeDS/policy/basic_policy.hpp>

namespace md::detail {

/**
 * Traversal policy that returns nodes in a line by line fashion. In forward order the nodes will be retrieved from left
 * to right, and from top to bottom.  Please note that this iterator is intended to be usedforward only (incremented
 * only). Reverse order iteration is possible (and tested) though it will imply severe performance drop.
 */
template <typename Node, typename Allocator>
class breadth_first_impl final
        : public basic_policy<breadth_first_impl<Node, Allocator>, Node, Allocator> {

    using super          = basic_policy<breadth_first_impl, Node, Allocator>;
    using allocator_type = typename std::allocator_traits<Allocator>::template rebind_alloc<std::decay_t<Node>>;

    private:
    std::deque<Node*, Allocator> open_nodes = manage_initial_status();
    allocator_type allocator;

    public:
    using basic_policy<breadth_first_impl, Node, Allocator>::basic_policy;

    // Formward puhes into open back and pops front
    Node* increment_impl() {
        if (this->open_nodes.empty()) {
            return nullptr;
        }
        // Get element to be returned
        Node* result = this->open_nodes.front();
        // Manage next sibling replacement in queue
        Node* sibling = result->get_next_sibling();
        if (sibling != nullptr && result != this->root) {
            this->open_nodes.front() = sibling;
        } else {
            this->open_nodes.pop_front();
        }
        // Push back its first child
        if (result->has_children()) {
            this->open_nodes.push_back(result->get_first_child());
        }
        return result;
    }

    Node* decrement_impl() {
        // Delete the child of current node from open_nodes
        if (this->current->has_children()) {
            assert(this->open_nodes.back()->get_parent() == this->current);
            // Delete the child of the previous node from open_nodes (invariants garantee that it is the last element)
            this->open_nodes.pop_back();
        }
        // Delete next sibling of the current node from open_nodes
        if (this->current != this->root && this->current->get_next_sibling() != nullptr) {
            assert(this->open_nodes.front()->get_prev_sibling() == this->current);
            this->open_nodes.pop_front();
        }
        // Calculate the previous element
        Node* result = left_branch_node(*this->current, *this->root);
        if (result == nullptr && this->current != this->root) {
            result = same_row_rightmost(*this->current->get_parent(), *this->root);
        }
        // Update queue
        this->open_nodes.push_front(this->current);
        return result;
    }

    Node* go_first_impl() {
        this->open_nodes.clear();
        this->open_nodes.push_back(this->root);
        return this->increment_impl();
    }

    Node* go_last_impl() {
        this->open_nodes.clear();
        return deepest_rightmost_child(*this->root);
    }

    std::deque<Node*, Allocator> manage_initial_status() {
        std::deque<Node*, Allocator> result;
        if (this->current == nullptr) {
            return result;
        }
        Node* node;
        auto process_child = [&]() {
            if (node->has_children()) {
                result.push_back(node->get_first_child());
            }
        };
        // Manage next_sibling replacement
        if (!this->current->is_last_child()) {
            result.push_back(this->current->get_next_sibling());
        }
        // Manage right elements
        node = right_branch_node(*this->current->get_parent(), *this->root);
        while (node != nullptr) {
            process_child();
            node = right_branch_node(*node, *this->root);
        }
        // Manage lower row, left elements
        node = same_row_leftmost(*this->current, *this->root);
        while (node != nullptr && node != this->current) {
            process_child();
            node = right_branch_node(*node, *this->root);
        }
        // Manage current node child
        node = this->current;
        process_child();
        return std::move(result);
    }

    void update(Node& current, Node* replacement) {
        // Delete child of the previous nodes from open_nodes.
        if (current.has_children()) {
            assert(this->open_nodes.back()->get_parent() == &current);
            this->open_nodes.pop_back();
        }
        // Push from back the children of the replacement node.
        Node* child = replacement != nullptr
            ? replacement->get_first_child()
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
    // What needed is inherited.
};
} // namespace md::policy
