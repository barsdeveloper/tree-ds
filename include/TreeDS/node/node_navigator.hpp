#pragma once

#include <type_traits>

#include <TreeDS/node/navigator_base.hpp>
#include <TreeDS/utility.hpp>

namespace md {

template <typename Node>
class node_navigator : public navigator_base<node_navigator<Node>, Node> {

    template <typename>
    friend class node_navigator;

    public:
    node_navigator() {
    }

    template <typename N, typename = std::enable_if_t<std::is_same_v<std::decay_t<N>, std::decay_t<Node>>>>
    node_navigator(const node_navigator<N>& other) :
            node_navigator(const_cast<Node*>(other.root), other.is_subtree) {
    }

    node_navigator(Node* root, bool is_subtree = true) :
            navigator_base<node_navigator<Node>, Node>(root, is_subtree) {
    }

    template <typename N, typename = std::enable_if_t<std::is_same_v<std::decay_t<N>, std::decay_t<Node>>>>
    node_navigator& operator=(const node_navigator<N>& other) {
        this->is_subtree = other.is_subtree;
        this->root       = const_cast<Node*>(other.root);
    }
};

} // namespace md
