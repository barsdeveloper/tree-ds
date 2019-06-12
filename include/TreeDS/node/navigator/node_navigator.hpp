#pragma once

#include <type_traits>

#include <TreeDS/node/navigator/navigator_base.hpp>
#include <TreeDS/utility.hpp>

namespace md {

template <typename NodePtr>
class node_navigator : public navigator_base<node_navigator<NodePtr>, NodePtr> {

    /*   ---   FRIENDS   ---   */
    template <typename>
    friend class node_navigator;

    /*   ---   CONSTRUCTORS   ---   */
    public:
    using navigator_base<node_navigator<NodePtr>, NodePtr>::navigator_base;

    template <typename OtherNodePTr, typename = std::enable_if_t<is_const_cast_equivalent<OtherNodePTr, NodePtr>>>
    node_navigator(const node_navigator<OtherNodePTr>& other) :
            node_navigator(const_cast<NodePtr>(other.root), other.is_subtree) {
    }

    /*   ---   ASSIGNMENT   ---   */
    template <typename OtherNodePtr, typename = std::enable_if_t<is_const_cast_equivalent<OtherNodePtr, NodePtr>>>
    node_navigator& operator=(const node_navigator<OtherNodePtr>& other) {
        this->is_subtree = other.is_subtree;
        this->root       = const_cast<NodePtr>(other.root);
    }
};

} // namespace md
