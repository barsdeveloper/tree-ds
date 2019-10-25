#pragma once

#include <TreeDS/node/navigator/navigator_base.hpp>
#include <TreeDS/utility.hpp>

namespace md {

template <typename NodePtr, typename Predicate>
class node_pred_navigator : public navigator_base<node_pred_navigator<NodePtr, Predicate>, NodePtr> {

    /*   ---   FRIENDS   ---   */
    template <typename, typename>
    friend class node_pred_navigator;

    /*   ---   TYPES   ---   */
    public:
    using typename navigator_base<node_pred_navigator, NodePtr>::node_pointer;
    using typename navigator_base<node_pred_navigator, NodePtr>::node_type;

    /*   ---   ATTRIBUTES   ---   */
    protected:
    std::optional<Predicate> predicate; // Not optional, just wrapped so to make it default constructible when lambda

    /*   ---   CONSTRUCTORS   ---   */
    public:
    node_pred_navigator() {
    }

    template <
        typename OtherNodePtr,
        typename = std::enable_if_t<std::is_convertible_v<OtherNodePtr, NodePtr>>>
    node_pred_navigator(OtherNodePtr current, Predicate predicate) :
            navigator_base<node_pred_navigator, NodePtr>(current),
            predicate(predicate) {
    }

    template <
        typename OtherNodePtr,
        typename = std::enable_if_t<std::is_convertible_v<OtherNodePtr, NodePtr>>>
    node_pred_navigator(NodePtr root, Predicate predicate) :
            navigator_base<node_pred_navigator, NodePtr>(root),
            predicate(predicate) {
    }

    /*   ---   ASSIGNMENT   ---   */
    node_pred_navigator& operator=(const node_pred_navigator& other) {
        this->navigator_base<node_pred_navigator, NodePtr>::operator=(other);
        // predicate may not be assignable
        return *this;
    }

    template <typename OtherNodePtr, typename = std::enable_if_t<std::is_convertible_v<OtherNodePtr, NodePtr>>>
    node_pred_navigator& operator=(const node_pred_navigator<OtherNodePtr, Predicate>& other) {
        this->navigator_base<node_pred_navigator, NodePtr>::operator=(other);
        return *this->cast();
    }

    /*   ---   METHODS   ---   */
    public:
    NodePtr get_prev_sibling(NodePtr node) {
        // Call the "vanilla" get_prev_sibling()
        NodePtr result = this->navigator_base<node_pred_navigator, NodePtr>::get_prev_sibling(node);
        while (result && !(*this->predicate)(result)) {
            // Then keep discarding siblings until a valid node appears
            result = this->navigator_base<node_pred_navigator, NodePtr>::get_prev_sibling(node);
        }
        return result;
    }

    NodePtr get_next_sibling(NodePtr node) {
        // Call the "vanilla" get_next_sibling()
        NodePtr result = this->navigator_base<node_pred_navigator, NodePtr>::get_next_sibling(node);
        while (result && !(*this->predicate)(result)) {
            // Then keep discarding siblings until a valid node appears
            result = this->navigator_base<node_pred_navigator, NodePtr>::get_next_sibling(result);
        }
        return result;
    }

    NodePtr get_first_child(NodePtr node) {
        // Call the "vanilla" get_first_child()
        NodePtr result = this->navigator_base<node_pred_navigator, NodePtr>::get_first_child(node);
        // Filter using get_next_sibling
        return result && !(*this->predicate)(result)
            ? this->get_next_sibling(result)
            : result;
    }

    NodePtr get_last_child(NodePtr node) {
        // Call the "vanilla" get_last_child()
        NodePtr result = this->navigator_base<node_pred_navigator, NodePtr>::get_last_child(node);
        // Filter using get_prev_sibling
        return result && !(*this->predicate)(result)
            ? this->get_prev_sibling(result)
            : result;
    }

    template <
        typename N = NodePtr,
        typename   = std::enable_if_t<is_binary_node_pointer<N>>>
    NodePtr get_left_child(N node) {
        // Call the "vanilla" get_left_child()
        NodePtr result = this->navigator_base<node_pred_navigator, NodePtr>::get_left_child(node);
        // Return nothing if fails the predicate
        return result && !(*this->predicate)(result)
            ? N()
            : result;
    }

    template <
        typename N = NodePtr,
        typename   = std::enable_if_t<is_binary_node_pointer<N>>>
    NodePtr get_right_child(N node) {
        // Call the "vanilla" get_right_child()
        NodePtr result = this->navigator_base<node_pred_navigator, NodePtr>::get_right_child(node);
        // Return nothing if fails the predicate
        return result && !(*this->predicate)(result)
            ? N()
            : result;
    }

    bool is_valid(NodePtr node) {
        return this->predicate(*node);
    }
};

} // namespace md
