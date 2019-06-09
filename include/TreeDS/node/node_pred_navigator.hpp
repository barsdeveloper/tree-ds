#pragma once

#include <TreeDS/node/navigator_base.hpp>
#include <TreeDS/utility.hpp>

namespace md {

template <typename Node, typename Predicate>
class node_pred_navigator : public navigator_base<node_pred_navigator<Node, Predicate>, Node> {

    template <typename, typename>
    friend class node_pred_navigator;

    protected:
    Predicate predicate;

    public:
    template <typename Pred = Predicate, typename = std::enable_if_t<std::is_default_constructible_v<Pred>>>
    node_pred_navigator() :
            predicate() {
    }

    template <
        typename OtherNode,
        typename = std::enable_if_t<std::is_same_v<std::decay_t<OtherNode>, std::decay_t<Node>>>>
    node_pred_navigator(const node_pred_navigator<OtherNode, Predicate>& other) :
            node_pred_navigator(const_cast<Node*>(other.root, other.is_subtree)) {
    }

    node_pred_navigator(Node* root, Predicate predicate, bool is_subtree = true) :
            navigator_base<node_pred_navigator<Node, Predicate>, Node>(root, is_subtree),
            predicate(predicate) {
    }

    node_pred_navigator operator=(const node_pred_navigator& other) {
        return this->operator=<Node>(other);
    }

    template <
        typename OtherNode, typename = std::enable_if_t<std::is_same_v<std::decay_t<OtherNode>, std::decay_t<Node>>>>
    node_pred_navigator operator=(const node_pred_navigator<OtherNode, Predicate>& other) {
        this->is_subtree = other.is_subtree;
        this->root       = const_cast<Node*>(other.root);
        return *this;
    }

    public:
    Node* get_prev_sibling(Node& node) const {
        Node* result = this->navigator_base<node_pred_navigator, Node>::get_prev_sibling(node);
        while (result && !this->predicate(*result)) {
            result = this->navigator_base<node_pred_navigator, Node>::get_prev_sibling(*result);
        }
        return result;
    }

    Node* get_next_sibling(Node& node) const {
        Node* result = this->navigator_base<node_pred_navigator, Node>::get_next_sibling(node);
        while (result && !this->predicate(*result)) {
            result = this->navigator_base<node_pred_navigator, Node>::get_next_sibling(*result);
        }
        return result;
    }

    Node* get_first_child(Node& node) const {
        Node* result = this->navigator_base<node_pred_navigator, Node>::get_first_child(node);
        return result && !this->predicate(*result)
            ? this->get_next_sibling(*result)
            : result;
    }

    Node* get_last_child(Node& node) const {
        Node* result = this->navigator_base<node_pred_navigator, Node>::get_last_child(node);
        return result && !this->predicate(*result)
            ? this->get_prev_sibling(*result)
            : result;
    }

    template <typename N = Node, typename = std::enable_if_t<is_same_template<std::decay_t<N>, binary_node<void>>>>
    Node* get_left_child(N& node) const {
        Node* result = node.get_left_child();
        if (result && !this->predicate(*result)) {
            return nullptr;
        }
        return result;
    }

    template <typename N = Node, typename = std::enable_if_t<is_same_template<std::decay_t<N>, binary_node<void>>>>
    Node* get_right_child(N& node) const {
        Node* result = node.get_right_child();
        if (result && !this->predicate(*result)) {
            return nullptr;
        }
        return result;
    }
};

} // namespace md
