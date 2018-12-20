#pragma once

#include <deque>
#include <functional>
#include <memory> // std::allocator

#include <TreeDS/node/node.hpp>

namespace md {

/**
 * Traversal policy that returns nodes in a line by line fashion. In forward order the nodes will be retrieved from left
 * to right, and from top to bottom.  Please note that this iterator is intended to be usedforward only (incremented
 * only). Reverse order iteration is possible (and tested) though it will imply severe performance drop.
 */
template <typename Node, typename Allocator = std::allocator<Node>>
class breadth_first final {

    private:
    std::deque<const Node*, Allocator> open_nodes{};
    Allocator allocator;

    public:
    breadth_first(const Allocator& allocator = Allocator()) :
            allocator(allocator) {
    }

    // formward puhes into open back and pops front
    const Node* increment(const Node&) {
        if (open_nodes.empty()) return nullptr;
        // pop front the element to return
        const Node* result = open_nodes.front();
        open_nodes.pop_front();
        const Node* first = result->get_first_child();
        // push back its children
        if (first) {
            keep_calling(
                *first,
                [&](const Node& node) {
                    open_nodes.push_back(&node);
                    return node.get_next_sibling();
                });
        }
        return result;
    }

    const Node* decrement(const Node& from) {
        // calculate the previous element
        const Node* result = prev_branch_sibling(from);
        if (!result) {
            result = upper_row_rightmost(from);
        }
        // delete the children of current node from open_nodes
        if (from.get_first_child() != nullptr) {
            // pop back the children of the previous element
            const Node* last = open_nodes.back();
            while (!open_nodes.empty() && last->get_parent() == &from) {
                open_nodes.pop_back();
                last = open_nodes.back();
            }
        }
        // push front the current (so that it will be the next from a forward iterator POV
        open_nodes.push_front(&from);
        return result;
    }

    const Node* go_first(const Node& root) {
        open_nodes.clear();
        open_nodes.push_back(&root);
        return increment(root);
    }

    const Node* go_last(const Node& root) {
        open_nodes.clear();
        return deepest_rightmost_child(root);
    }

    void update(const Node& current, const Node& replacement) {
        // delete children of the previous nodes from open_nodes (invariants garantee that they are the last elements)
        while (!open_nodes.empty()) {
            const Node* last = open_nodes.back();
            if (last->get_parent() == &current) {
                open_nodes.pop_back();
                last = open_nodes.back();
            } else {
                break;
            }
        }
        // push from back the children of the replacement node
        const Node* child = replacement.get_first_child();
        while (child != nullptr) {
            open_nodes.push_back(child);
            child = child->get_next_sibling();
        }
    }
};

} // namespace ds
