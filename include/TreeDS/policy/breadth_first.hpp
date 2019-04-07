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
class breadth_first_impl final : public basic_policy<breadth_first_impl<Node, Allocator>, Node, Allocator> {

    using allocator_type = typename std::allocator_traits<Allocator>::template rebind_alloc<std::decay_t<Node>>;

    private:
    std::deque<Node*, Allocator> open_nodes {};
    allocator_type allocator;

    public:
    using basic_policy<breadth_first_impl<Node, Allocator>, Node, Allocator>::basic_policy;

    /*
         * Create a policy that does not point at the beginning of the iteration but somewhere into the range. This
         * means that we must reconstruct open_nodes deque like if we started at the beginning and arrived at current.
         */
    breadth_first_impl(Node* root, Node* current, const Allocator& allocator) :
            basic_policy<breadth_first_impl, Node, Allocator>(root, current, allocator) {
        if (current == nullptr) {
            return;
        }
        Node* node;
        auto process_child = [&]() {
            if (node->has_children()) {
                this->open_nodes.push_back(node->get_first_child());
            }
        };
        // Manage next_sibling replacement
        if (!this->current->is_last_child()) {
            this->open_nodes.push_back(this->current->get_next_sibling());
        }
        // Manage right elements
        node = right_branch_node(*current->get_parent(), *root);
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
    }

    // formward puhes into open back and pops front
    Node* increment_impl() {
        if (this->open_nodes.empty()) {
            return nullptr;
        }
        // get element to be returned
        Node* result = this->open_nodes.front();
        // manage next sibling replacement in queue
        Node* sibling = result->get_next_sibling_limit(*this->root);
        if (sibling) {
            this->open_nodes.front() = sibling;
        } else {
            this->open_nodes.pop_front();
        }
        // push back its first child
        Node* first = result->get_first_child();
        if (first != nullptr) {
            this->open_nodes.push_back(first);
        }
        return result;
    }

    Node* decrement_impl() {
        // delete the child of current node from open_nodes
        if (this->current->has_children()) {
            assert(this->open_nodes.back()->get_parent() == this->current);
            // delete child of the previous node from open_nodes (invariants garantee that it is the last element)
            this->open_nodes.pop_back();
        }
        // delete next sibling of the current node from open_nodes
        if (this->current->get_next_sibling_limit(*this->root) != nullptr) {
            assert(this->open_nodes.front()->get_prev_sibling() == this->current);
            this->open_nodes.pop_front();
        }
        // calculate the previous element
        Node* result = left_branch_node(*this->current, *this->root);
        if (result == nullptr && !this->current->is_root_limit(*this->root)) {
            result = same_row_rightmost(*this->current->get_parent(), *this->root);
        }
        // update queue
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

    void update(Node& current, Node* replacement) {
        // delete child of the previous nodes from open_nodes
        if (current.has_children()) {
            assert(this->open_nodes.back()->get_parent() == &current);
            this->open_nodes.pop_back();
        }
        // push from back the children of the replacement node
        Node* child = replacement != nullptr
            ? replacement->get_first_child()
            : nullptr;
        if (child != nullptr) {
            this->open_nodes.push_back(child);
        }
        this->basic_policy<breadth_first_impl, Node, Allocator>::update(current, replacement);
    }
};

} // namespace md::detail

namespace md::policy {

struct breadth_first : detail::tag<detail::breadth_first_impl> {
    // what needed is inherited
};

} // namespace md::policy
