#pragma once

#include <deque>
#include <memory> // std::allocator

#include <TreeDS/node/node.hpp>
#include <TreeDS/policy/policy.hpp>

namespace md {

namespace detail {

    /**
 * Traversal policy that returns nodes in a line by line fashion. In forward order the nodes will be retrieved from left
 * to right, and from top to bottom.  Please note that this iterator is intended to be usedforward only (incremented
 * only). Reverse order iteration is possible (and tested) though it will imply severe performance drop.
 */
    template <typename Node, typename Allocator = std::allocator<Node>>
    class breadth_first_impl final : policy<Node> {

        using allocator_type = typename std::allocator_traits<Allocator>::template rebind_alloc<Node>;

        private:
        std::deque<const Node*, Allocator> open_nodes {};
        allocator_type allocator;

        public:
        breadth_first_impl() :
                breadth_first_impl(nullptr, Allocator()) {
        }

        breadth_first_impl(const Node* root, const Allocator& allocator = Allocator()) :
                policy<Node>(root),
                allocator(allocator) {
        }

        // formward puhes into open back and pops front
        const Node* increment(const Node&) {
            if (this->open_nodes.empty()) {
                return nullptr;
            }
            // get element to be returned
            const Node* result = this->open_nodes.front();
            // manage next sibling replacement in queue
            const Node* sibling = result->get_next_sibling_limit(*this->root);
            if (sibling) {
                this->open_nodes.front() = sibling;
            } else {
                this->open_nodes.pop_front();
            }
            // push back its first child
            const Node* first = result->get_first_child();
            if (first != nullptr) {
                this->open_nodes.push_back(first);
            }
            return result;
        }

        const Node* decrement(const Node& from) {
            // delete the child of current node from open_nodes
            if (from.get_first_child() != nullptr) {
                assert(this->open_nodes.back()->get_parent() == &from);
                // delete child of the previous node from open_nodes (invariants garantee that it is the last element)
                this->open_nodes.pop_back();
            }
            // delete next sibling of the current node from open_nodes
            if (from.get_next_sibling_limit(*this->root) != nullptr) {
                assert(this->open_nodes.front()->get_prev_sibling() == &from);
                this->open_nodes.pop_front();
            }
            // calculate the previous element
            const Node* result = prev_branch_sibling(from, *this->root);
            if (result == nullptr) {
                result = upper_row_rightmost(from, *this->root);
            }
            // update queue
            this->open_nodes.push_front(&from);
            return result;
        }

        const Node* go_first(const Node& root) {
            this->open_nodes.clear();
            this->open_nodes.push_back(&root);
            this->root = &root;
            return increment(root);
        }

        const Node* go_last(const Node& root) {
            this->open_nodes.clear();
            this->root = &root;
            return deepest_rightmost_child(root);
        }

        void update(const Node& current, const Node* replacement) {
            // delete child of the previous nodes from open_nodes
            if (current.get_first_child()) {
                assert(this->open_nodes.back()->get_parent() == &current);
                this->open_nodes.pop_back();
            }
            // push from back the children of the replacement node
            const Node* child = replacement != nullptr
                ? replacement->get_first_child()
                : nullptr;
            if (child != nullptr) {
                this->open_nodes.push_back(child);
            }
        }
    };

} // namespace detail

struct breadth_first {
    template <typename Node, typename Allocator>
    detail::breadth_first_impl<Node, Allocator> get_instance(
        const Node* root,
        const Allocator& allocator = Allocator()) const {
        return breadth_first(root, allocator);
    }
};

} // namespace md
