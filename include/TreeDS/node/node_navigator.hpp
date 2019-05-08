#pragma once

#include <type_traits>

#include <TreeDS/utility.hpp>

namespace md {

template <typename Node>
class node_navigator {

    template <typename>
    friend class node_navigator;

    protected:
    bool is_subtree = false;
    Node* root      = nullptr;

    public:
    node_navigator() {
    }

    template <typename N, typename = std::enable_if_t<std::is_same_v<std::decay_t<N>, std::decay_t<Node>>>>
    node_navigator(const node_navigator<N>& other) :
            node_navigator(const_cast<Node*>(other.root), other.is_subtree) {
    }

    node_navigator(Node* root, bool is_subtree = true) :
            is_subtree(is_subtree),
            root(root) {
    }

    template <typename N, typename = std::enable_if_t<std::is_same_v<std::decay_t<N>, std::decay_t<Node>>>>
    node_navigator& operator=(const node_navigator<N>& other) {
        this->is_subtree = other.is_subtree;
        this->root       = other.root;
    }

    protected:
    template <bool Left>
    Node* get_other_branch(Node& node) const {
        int relative_level  = 0;
        Node* deepest_child = &node;
        Node* crossed;
        do {
            // Climb up the tree until a node with a sibling is found, then return that sibling
            crossed = keep_calling(
                // From
                *deepest_child,
                // Keep calling
                [this](Node& node) {
                    return this->get_parent(node);
                },
                // Until
                [&](Node& child, Node&) {
                    --relative_level;
                    return Left
                        ? !this->is_first_child(child)
                        : !this->is_last_child(child);
                },
                // Then return
                [&](Node& child, Node&) {
                    // Increment level because it considers the previous value (first argument)
                    ++relative_level;
                    return Left
                        // Always present because it is not the first child
                        ? this->get_prev_sibling(child)
                        // Aways present because it is not the last child
                        : this->get_next_sibling(child);
                });
            if (crossed == this->root) {
                return nullptr;
            } else if (relative_level == 0) {
                return crossed;
            }
            // Descend the tree following the last children until level is reached or no more children
            deepest_child = keep_calling(
                // From
                *crossed,
                // Keep calling
                [this](Node& node) {
                    return Left
                        ? this->get_last_child(node)
                        : this->get_first_child(node);
                },
                // Until
                [&](Node&, Node&) {
                    ++relative_level;
                    return relative_level >= 0;
                },
                // The return
                [](Node&, Node& child) {
                    return &child;
                });
        } while (relative_level < 0);
        return deepest_child;
    }

    template <bool Left>
    Node* get_row_extremum(Node& from) const {
        if (&from == this->root) {
            return &from;
        }
        int relative_level = 0;
        Node* deepest_extremum_child;
        // Climb the tree up to the root
        Node* breanch_crossed = keep_calling(
            // From
            from,
            // Keep calling
            [this](Node& node) {
                return this->get_parent(node);
            },
            // Until
            [&](Node&, Node& parent) {
                --relative_level;
                return &parent == this->root;
            },
            // Then return
            [](Node&, Node& root) {
                return &root;
            });
        do {
            // Descend the tree traversing extremum children
            deepest_extremum_child = keep_calling(
                // From
                *breanch_crossed,
                // Keep calling
                [this](Node& node) {
                    return Left
                        ? this->get_first_child(node)
                        : this->get_last_child(node);
                },
                // Until
                [&](Node&, Node&) {
                    ++relative_level;
                    return relative_level >= 0;
                },
                // Then return
                [](Node&, Node& child) {
                    return &child;
                });
            if (relative_level == 0) {
                break;
            }
            // Go to the node that is on the other direction branch and at the same level.
            breanch_crossed = this->template get_other_branch<!Left>(*deepest_extremum_child);
        } while (relative_level < 0);
        return deepest_extremum_child;
    }

    template <bool Left>
    Node* get_highest_leaf() const {
        return keep_calling(
            *this->root,
            [this](Node& node) {
                return Left
                    ? this->get_first_child(node)
                    : this->get_last_child(node);
            });
    }

    template <bool Left>
    Node* get_deepest_extremum_child() const {
        int current_level  = 0;
        int deepest_level  = 0;
        Node* current_node = this->root;
        Node* deepest_node = this->root;
        do {
            // Descend the tree keeping right (last child)
            current_node = keep_calling(
                // From
                *current_node,
                // Keep calling
                [this](Node& node) {
                    return Left
                        ? this->get_first_child(node)
                        : this->get_last_child(node);
                },
                // Until
                [&](Node&, Node&) {
                    ++current_level;
                    return false;
                },
                // Then return
                std::function<Node*(Node&, Node&)>()); // Never called ("Until" lambda retuns always false).
            if (current_level > deepest_level) {
                deepest_level = current_level;
                deepest_node  = current_node;
            }
            // Go to the near node, at the same level
            current_node = this->template get_other_branch<!Left>(*current_node);
        } while (current_node != nullptr);
        return deepest_node;
    }

    public:
    bool is_root(Node& node) const {
        if (this->is_subtree && &node == this->root) {
            return true;
        }
        return node.is_root();
    }

    bool is_first_child(Node& node) const {
        if (this->is_subtree) {
            if (&node == this->root) {
                return false;
            }
        }
        return node.is_first_child();
    }

    bool is_last_child(Node& node) const {
        if (this->is_subtree && &node == this->root) {
            return false;
        }
        return node.is_last_child();
    }

    template <typename N = Node, typename = std::enable_if_t<is_same_template<std::decay_t<N>, binary_node<void>>>>
    bool is_left_child(N& node) const {
        if (this->is_subtree) {
            return &node != this->root && node.is_left_child();
        } else {
            return node.is_left_child();
        }
    }

    template <typename N = Node, typename = std::enable_if_t<is_same_template<std::decay_t<N>, binary_node<void>>>>
    bool is_right_child(N& node) const {
        if (this->is_subtree) {
            return &node != this->root && node.is_right_child();
        } else {
            return node.is_right_child();
        }
    }

    Node* get_root() const {
        return this->root;
    }

    Node* get_parent(Node& node) const {
        if (this->is_subtree) {
            if (&node == this->root) return nullptr;
        }
        return node.get_parent();
    }

    Node* get_prev_sibling(Node& node) const {
        if (this->is_subtree) {
            if (&node == this->root) {
                return nullptr;
            }
        }
        return node.get_prev_sibling();
    }

    Node* get_next_sibling(Node& node) const {
        if (this->is_subtree) {
            if (&node == this->root) {
                return nullptr;
            }
        }
        return node.get_next_sibling();
    }

    Node* get_first_child(Node& node) const {
        return node.get_first_child();
    }

    Node* get_last_child(Node& node) const {
        return node.get_last_child();
    }

    Node* get_child(Node& node, std::size_t index) const {
        return node.get_child(index);
    }

    template <typename N = Node, typename = std::enable_if_t<is_same_template<std::decay_t<N>, binary_node<void>>>>
    Node* get_left_child(N& node) const {
        return node.get_left_child();
    }

    template <typename N = Node, typename = std::enable_if_t<is_same_template<std::decay_t<N>, binary_node<void>>>>
    Node* get_right_child(N& node) const {
        return node.get_right_child();
    }

    Node* get_left_branch(Node& node) const {
        return this->template get_other_branch<true>(node);
    }

    Node* get_right_branch(Node& node) const {
        return this->template get_other_branch<false>(node);
    }

    Node* get_same_row_leftmost(Node& from) const {
        return this->template get_row_extremum<true>(from);
    }

    Node* get_same_row_rightmost(Node& from) const {
        return this->template get_row_extremum<false>(from);
    }

    Node* get_highest_left_leaf() const {
        return this->template get_highest_leaf<true>();
    }

    Node* get_highest_right_leaf() const {
        return this->template get_highest_leaf<false>();
    }

    Node* get_deepest_leftmost_leaf() const {
        return this->template get_deepest_extremum_child<true>();
    }

    Node* get_deepest_rightmost_leaf() const {
        return this->template get_deepest_extremum_child<false>();
    }
};

} // namespace md
