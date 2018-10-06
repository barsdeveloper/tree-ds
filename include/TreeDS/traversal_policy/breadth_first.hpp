#pragma once

#include <deque>
#include <functional>

#include <TreeDS/node/node.hpp>

namespace ds {

/**
 * Traversal policy that returns nodes in a line by line fashion. In forward order the nodes will be retrieved from left
 * to right, and from top to bottom. Please note that this iterator is intended to be used forward only (incremented
 * only). Reverse order iteration is possible (and tested) though it will imply severve performance hits.
 */
template <typename Node>
class breadth_first final {

    private:
    std::deque<const Node*> open_nodes{};
    const Node* root = nullptr;

    public:
    breadth_first() = default;

    // formward puhes into open back and pops front
    const Node* increment() {
        if (open_nodes.empty()) return nullptr;
        // pop front the element to return
        const Node* result = open_nodes.front();
        open_nodes.pop_front();
        const Node* first = result->get_first_child();
        // push back the children
        if (first) {
            keep_calling(
                *first, // descent takes care to not call the next lambda if null
                [&](const Node& node) {
                    open_nodes.push_back(&node);
                    return node.get_next_sibling();
                });
        }
        return result;
    }

    const Node* decrement() {
        const Node* result;
        if (open_nodes.empty()) {
            result = deepest_rightmost_child(*root);
            open_nodes.push_front(result);
            return result;
        } else {
            const Node* front = open_nodes.front(); // never nullptr
            result            = prev_branch_sibling(*front);
            if (!result) {
                result = upper_row_rightmost(*front);
            }
        }
        // pop back the children
        if (result) {
            const Node* last = open_nodes.back();
            while (!open_nodes.empty() && last->get_parent() == result) {
                open_nodes.pop_back();
                last = open_nodes.back();
            }
            // push front the result to return
            open_nodes.push_front(result);
        }
        return result;
    }

    const Node* go_first(const Node& root) {
        this->root = &root;
        open_nodes.clear();
        open_nodes.push_back(&root);
        return increment();
    }

    const Node* go_last(const Node& root) {
        this->root = &root;
        open_nodes.clear();
        return decrement();
    }
};

} // namespace ds
