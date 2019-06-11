#pragma once

#include <type_traits>

#include <TreeDS/node/node_pred_navigator.hpp>
#include <TreeDS/utility.hpp>

namespace md {

template <typename NodeAllocator, typename NodeWrap, typename Predicate>
class generative_pred_navigator : public node_pred_navigator<NodeWrap, Predicate> {

    /*   ---   FRIENDS   ---   */
    template <typename>
    friend class node_navigator;

    /*   ---   TYPES   ---   */
    using node_ptr_t = decltype(std::declval<NodeWrap>().referred);

    /*   ---   ATTRIBUTES   ---   */
    NodeAllocator& allocator;
    std::decay_t<std::remove_pointer_t<NodeWrap>>* generated;
    node_pred_navigator<node_ptr_t, Predicate> ptr_node_navigator;

    /*   ---   CONSTRUCTORS   ---   */
    public:
    using node_pred_navigator<NodeWrap, Predicate>::node_pred_navigator;

    template <
        typename OtherNodePtr,
        typename = std::enable_if_t<is_const_cast_equivalent<OtherNodePtr, NodeWrap>>>
    generative_pred_navigator(const generative_pred_navigator<NodeAllocator, OtherNodePtr, Predicate>& other) :
            node_pred_navigator<NodeWrap, Predicate>(const_cast<NodeWrap>(other.root), other.is_subtree),
            allocator(other.allocator),
            generated(other.generated) {
    }

    generative_pred_navigator(NodeAllocator allocator, NodeWrap referred, NodeWrap generated, bool is_subtree = true) :
            navigator_base<generative_pred_navigator<NodeAllocator, NodeWrap, Predicate>, NodeWrap>(referred, is_subtree),
            allocator(allocator),
            generated(generated) {
    }

    /*   ---   ASSIGNMENT   ---   */
    template <typename OtherNodePtr, typename = std::enable_if_t<is_const_cast_equivalent<OtherNodePtr, NodeWrap>>>
    generative_pred_navigator& operator=(
        const generative_pred_navigator<NodeAllocator, OtherNodePtr, Predicate>& other) {
        this->is_subtree = other.is_subtree;
        this->root       = const_cast<NodeWrap>(other.root);
        this->generated  = other.generated;
    }

    /*   ---   METHODS   ---   */
    public:
    NodeWrap get_prev_sibling(NodeWrap node) const {
        assert(node.generated);
        node_ptr_t referred  = this->ptr_node_navigator.get_prev_sibling(node.referred);
        node_ptr_t generated = this->ptr_node_navigator.get_prev_sibling(node.generated);
    }

    NodeWrap get_next_sibling(NodeWrap node) const {
        assert(node.generated);
        NodeWrap result = this->node_pred_navigator<NodeWrap, Predicate>::get_next_sibling(node);
    }

    NodeWrap get_first_child(NodeWrap node) const {
        assert(node.generated);
        NodeWrap result = this->node_pred_navigator<NodeWrap, Predicate>::get_first_child(node);
    }

    NodeWrap get_last_child(NodeWrap node) const {
        assert(node.generated);
        NodeWrap result = this->node_pred_navigator<NodeWrap, Predicate>::get_last_child(node);
    }

    template <
        typename N = NodeWrap,
        typename   = std::enable_if_t<is_same_template<std::decay_t<std::remove_pointer_t<N>>, binary_node<void>>>>
    NodeWrap get_left_child(N node) const {
        assert(node.generated);
        NodeWrap result = this->node_pred_navigator<NodeWrap, Predicate>::get_left_child(node);
    }

    template <
        typename N = NodeWrap,
        typename   = std::enable_if_t<is_same_template<std::decay_t<std::remove_pointer_t<N>>, binary_node<void>>>>
    NodeWrap get_right_child(N node) const {
        assert(node.generated);
        NodeWrap result = this->node_pred_navigator<NodeWrap, Predicate>::get_right_child(node);
    }
};

} // namespace md
