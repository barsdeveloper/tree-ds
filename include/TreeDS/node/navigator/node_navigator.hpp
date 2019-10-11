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

    template <typename OtherNodePtr, typename = std::enable_if_t<std::is_convertible_v<OtherNodePtr, NodePtr>>>
    node_navigator(const node_navigator<OtherNodePtr>& other) :
            node_navigator(other.root) {
    }

    /*   ---   ASSIGNMENT   ---   */
    template <typename OtherNodePtr, typename = std::enable_if_t<std::is_convertible_v<OtherNodePtr, NodePtr>>>
    node_navigator& operator=(const node_navigator<OtherNodePtr>& other) {
        this->navigator_base<node_navigator, NodePtr>::operator==(other);
    }
};

} // namespace md
