#pragma once

#include <TreeDS/node/navigator/navigator_base.hpp>
#include <TreeDS/utility.hpp>

namespace md {

template <typename NodePtr, typename Predicate>
class node_pred_navigator : public navigator_base<node_pred_navigator<NodePtr, Predicate>, NodePtr> {

    /*   ---   FRIENDS   ---   */
    template <typename, typename>
    friend class node_pred_navigator;

    /*   ---   ATTRIBUTES   ---   */
    protected:
    Predicate predicate;

    /*   ---   CONSTRUCTORS   ---   */
    public:
    template <typename Pred = Predicate, typename = std::enable_if_t<std::is_default_constructible_v<Pred>>>
    node_pred_navigator() :
            predicate() {
    }

    template <
        typename OtherNodePtr,
        typename = std::enable_if_t<is_const_cast_equivalent<OtherNodePtr, NodePtr>>>
    node_pred_navigator(const node_pred_navigator<OtherNodePtr, Predicate>& other) :
            node_pred_navigator(const_cast<NodePtr*>(other.root, other.is_subtree)) {
    }

    node_pred_navigator(NodePtr root, Predicate predicate, bool is_subtree = true) :
            navigator_base<node_pred_navigator<NodePtr, Predicate>, NodePtr>(root, is_subtree),
            predicate(predicate) {
    }

    /*   ---   ASSIGNMENT   ---   */
    node_pred_navigator operator=(const node_pred_navigator& other) {
        return this->operator=<NodePtr>(other);
    }

    template <
        typename OtherNodePtr,
        typename = std::enable_if_t<is_const_cast_equivalent<OtherNodePtr, NodePtr>>>
    node_pred_navigator operator=(const node_pred_navigator<OtherNodePtr, Predicate>& other) {
        this->is_subtree = other.is_subtree;
        this->root       = const_cast<NodePtr>(other.root);
        return *this;
    }

    /*   ---   METHODS   ---   */
    public:
    NodePtr get_prev_sibling(NodePtr node) const {
        NodePtr result = this->navigator_base<node_pred_navigator, NodePtr>::get_prev_sibling(node);
        while (result && !this->predicate(*result)) {
            result = this->navigator_base<node_pred_navigator, NodePtr>::get_prev_sibling(result);
        }
        return result;
    }

    NodePtr get_next_sibling(NodePtr node) const {
        NodePtr result = this->navigator_base<node_pred_navigator, NodePtr>::get_next_sibling(node);
        while (result && !this->predicate(*result)) {
            result = this->navigator_base<node_pred_navigator, NodePtr>::get_next_sibling(result);
        }
        return result;
    }

    NodePtr get_first_child(NodePtr node) const {
        NodePtr result = this->navigator_base<node_pred_navigator, NodePtr>::get_first_child(node);
        return result && !this->predicate(*result)
            ? this->get_next_sibling(result)
            : result;
    }

    NodePtr get_last_child(NodePtr node) const {
        NodePtr result = this->navigator_base<node_pred_navigator, NodePtr>::get_last_child(node);
        return result && !this->predicate(*result)
            ? this->get_prev_sibling(result)
            : result;
    }

    template <typename N>
    NodePtr get_left_child(N node) const {
        NodePtr result = node.get_left_child();
        if (result && !this->predicate(*result)) {
            return nullptr;
        }
        return result;
    }

    template <typename N>
    NodePtr get_right_child(N node) const {
        NodePtr result = node.get_right_child();
        if (result && !this->predicate(*result)) {
            return nullptr;
        }
        return result;
    }
};

} // namespace md
