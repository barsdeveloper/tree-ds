#pragma once

#include <type_traits>

#include <TreeDS/node/navigator_base.hpp>
#include <TreeDS/utility.hpp>

namespace md {

template <typename NodePtr>
class node_navigator : public navigator_base<node_navigator<NodePtr>, NodePtr> {

    template <typename>
    friend class node_navigator;

    public:
    node_navigator() {
    }

    template <typename N, typename = std::enable_if_t<is_decay_pointed_same<N, NodePtr>>>
    node_navigator(const node_navigator<N>& other) :
            node_navigator(const_cast<NodePtr>(other.root), other.is_subtree) {
    }

    node_navigator(NodePtr root, bool is_subtree = true) :
            navigator_base<node_navigator<NodePtr>, NodePtr>(root, is_subtree) {
    }

    template <typename N, typename = std::enable_if_t<is_decay_pointed_same<N, NodePtr>>>
    node_navigator& operator=(const node_navigator<N>& other) {
        this->is_subtree = other.is_subtree;
        this->root       = const_cast<NodePtr>(other.root);
    }
};

} // namespace md
