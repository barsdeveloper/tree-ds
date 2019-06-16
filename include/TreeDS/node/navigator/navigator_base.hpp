#pragma once

#include <type_traits>
#include <utility> // std::declval

#include <TreeDS/utility.hpp>

namespace md {

template <typename Derived, typename NodePtr>
class navigator_base {

    /*   ---   FRIENDS   ---   */
    template <typename>
    friend class node_navigator;

    /*   ---   TYPES   ---   */
    using derived_this = const Derived*;

    /*   ---   ATTRIBUTES   ---   */
    protected:
    bool is_subtree = false;
    NodePtr root    = nullptr;

    /*   ---   CONSTRUCTORS   ---   */
    public:
    navigator_base() {
    }

    navigator_base(NodePtr root, bool is_subtree = true) :
            is_subtree(is_subtree),
            root(root) {
    }

    /*   ---   METHODS   ---   */
    protected:
    template <bool Left>
    NodePtr get_other_branch(NodePtr node) const {
        int relative_level    = 0;
        NodePtr deepest_child = node;
        NodePtr crossed;
        do {
            // Climb up the tree until a node with a sibling is found, then return that sibling
            crossed = keep_calling(
                // From
                deepest_child,
                // Keep calling
                [this](NodePtr node) {
                    return static_cast<derived_this>(this)->get_parent(node);
                },
                // Until
                [&](NodePtr child, NodePtr) {
                    --relative_level;
                    return Left
                        ? !static_cast<derived_this>(this)->is_first_child(child)
                        : !static_cast<derived_this>(this)->is_last_child(child);
                },
                // Then return
                [&](NodePtr child, NodePtr) {
                    // Increment level because it considers the previous value (first argument)
                    ++relative_level;
                    return Left
                        // Always present because it is not the first child
                        ? static_cast<derived_this>(this)->get_prev_sibling(child)
                        // Aways present because it is not the last child
                        : static_cast<derived_this>(this)->get_next_sibling(child);
                });
            if (crossed == this->root) {
                return nullptr;
            } else if (relative_level == 0) {
                return crossed;
            }
            // Descend the tree following the last children until level is reached or no more children
            deepest_child = keep_calling(
                // From
                crossed,
                // Keep calling
                [this](NodePtr node) {
                    return Left
                        ? static_cast<derived_this>(this)->get_last_child(node)
                        : static_cast<derived_this>(this)->get_first_child(node);
                },
                // Until
                [&](NodePtr, NodePtr) {
                    ++relative_level;
                    return relative_level >= 0;
                },
                // The return
                [](NodePtr, NodePtr child) {
                    return child;
                });
        } while (relative_level < 0);
        return deepest_child;
    }

    template <bool Left>
    NodePtr get_row_extremum(NodePtr from) const {
        if (from == this->root) {
            return from;
        }
        int relative_level = 0;
        NodePtr deepest_extremum_child;
        // Climb the tree up to the root
        NodePtr breanch_crossed = keep_calling(
            // From
            from,
            // Keep calling
            [this](NodePtr node) {
                return static_cast<derived_this>(this)->get_parent(node);
            },
            // Until
            [&](NodePtr, NodePtr parent) {
                --relative_level;
                return parent == this->root;
            },
            // Then return
            [](NodePtr, NodePtr root) {
                return root;
            });
        do {
            // Descend the tree traversing extremum children
            deepest_extremum_child = keep_calling(
                // From
                breanch_crossed,
                // Keep calling
                [this](NodePtr node) {
                    return Left
                        ? static_cast<derived_this>(this)->get_first_child(node)
                        : static_cast<derived_this>(this)->get_last_child(node);
                },
                // Until
                [&](NodePtr, NodePtr) {
                    ++relative_level;
                    return relative_level >= 0;
                },
                // Then return
                [](NodePtr, NodePtr child) {
                    return child;
                });
            if (relative_level == 0) {
                break;
            }
            // Go to the node that is on the other direction branch and at the same level
            breanch_crossed = static_cast<derived_this>(this)->template get_other_branch<!Left>(deepest_extremum_child);
        } while (relative_level < 0);
        return deepest_extremum_child;
    }

    template <bool Left>
    NodePtr get_highest_leaf() const {
        return keep_calling(
            this->root,
            [this](NodePtr node) {
                return Left
                    ? static_cast<derived_this>(this)->get_first_child(node)
                    : static_cast<derived_this>(this)->get_last_child(node);
            });
    }

    template <bool Left>
    NodePtr get_deepest_extremum_child() const {
        int current_level    = 0;
        int deepest_level    = 0;
        NodePtr current_node = this->root;
        NodePtr deepest_node = this->root;
        do {
            // Descend the tree keeping right (last child)
            current_node = keep_calling(
                // From
                current_node,
                // Keep calling
                [this](NodePtr node) {
                    return Left
                        ? static_cast<derived_this>(this)->get_first_child(node)
                        : static_cast<derived_this>(this)->get_last_child(node);
                },
                // Until
                [&](NodePtr, NodePtr) {
                    ++current_level;
                    return false;
                },
                // Then return
                std::function<NodePtr(NodePtr, NodePtr)>()); // Never called ("Until" lambda retuns always false)
            if (current_level > deepest_level) {
                deepest_level = current_level;
                deepest_node  = current_node;
            }
            // Go to the near node, at the same level
            current_node = static_cast<derived_this>(this)->template get_other_branch<!Left>(current_node);
        } while (current_node != nullptr);
        return deepest_node;
    }

    public:
    bool is_root(NodePtr node) const {
        return node == this->root;
    }

    bool is_first_child(NodePtr node) const {
        return !static_cast<derived_this>(this)->is_root(node) && node == node->get_parent()->get_first_child();
    }

    bool is_last_child(NodePtr node) const {
        return !static_cast<derived_this>(this)->is_root(node) && node == node->get_parent()->get_last_child();
    }

    template <
        typename N = NodePtr,
        typename   = std::enable_if_t<
            is_same_template<
                std::decay_t<std::remove_pointer_t<N>>,
                binary_node<void>>>>
    bool is_left_child(N node) const {
        return !static_cast<derived_this>(this)->is_root(node) && node == node->get_parent()->get_left_child();
    }

    template <
        typename N = NodePtr,
        typename   = std::enable_if_t<
            is_same_template<
                std::decay_t<std::remove_pointer_t<N>>,
                binary_node<void>>>>
    bool is_right_child(N node) const {
        return !static_cast<derived_this>(this)->is_root(node) && node == node->get_parent()->get_right_child();
    }

    NodePtr get_root() const {
        return this->root;
    }

    NodePtr get_parent(NodePtr node) const {
        return !static_cast<derived_this>(this)->is_root(node) ? node->get_parent() : nullptr;
    }

    NodePtr get_prev_sibling(NodePtr node) const {
        return !static_cast<derived_this>(this)->is_root(node) ? node->get_prev_sibling() : nullptr;
    }

    NodePtr get_next_sibling(NodePtr node) const {
        return !static_cast<derived_this>(this)->is_root(node) ? node->get_next_sibling() : nullptr;
    }

    NodePtr get_first_child(NodePtr node) const {
        return node->get_first_child();
    }

    NodePtr get_last_child(NodePtr node) const {
        return node->get_last_child();
    }

    NodePtr get_child(NodePtr node, std::size_t index) const {
        return node->get_child(index);
    }

    template <
        typename N = NodePtr,
        typename   = std::enable_if_t<
            is_same_template<
                std::decay_t<std::remove_pointer_t<N>>,
                binary_node<void>>>>
    NodePtr get_left_child(N node) const {
        return node->get_left_child();
    }

    template <
        typename N = NodePtr,
        typename   = std::enable_if_t<
            is_same_template<
                std::decay_t<std::remove_pointer_t<N>>,
                binary_node<void>>>>
    NodePtr get_right_child(N node) const {
        return node->get_right_child();
    }

    NodePtr get_left_branch(NodePtr node) const {
        return static_cast<derived_this>(this)->template get_other_branch<true>(node);
    }

    NodePtr get_right_branch(NodePtr node) const {
        return static_cast<derived_this>(this)->template get_other_branch<false>(node);
    }

    NodePtr get_same_row_leftmost(NodePtr from) const {
        return static_cast<derived_this>(this)->template get_row_extremum<true>(from);
    }

    NodePtr get_same_row_rightmost(NodePtr from) const {
        return static_cast<derived_this>(this)->template get_row_extremum<false>(from);
    }

    NodePtr get_highest_left_leaf() const {
        return static_cast<derived_this>(this)->template get_highest_leaf<true>();
    }

    NodePtr get_highest_right_leaf() const {
        return static_cast<derived_this>(this)->template get_highest_leaf<false>();
    }

    NodePtr get_deepest_leftmost_leaf() const {
        return static_cast<derived_this>(this)->template get_deepest_extremum_child<true>();
    }

    NodePtr get_deepest_rightmost_leaf() const {
        return static_cast<derived_this>(this)->template get_deepest_extremum_child<false>();
    }
};

} // namespace md
