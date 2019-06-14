#pragma once

#include <type_traits>

#include <TreeDS/node/multiple_node_pointer.hpp>
#include <TreeDS/node/navigator/node_pred_navigator.hpp>
#include <TreeDS/utility.hpp>

namespace md {

template <typename NodeAllocator, typename Predicate, typename TargetNodePtr, typename GeneratedNodePtr>
class generative_navigator
        : public node_pred_navigator<
              multiple_node_pointer<TargetNodePtr, GeneratedNodePtr>,
              Predicate> {

    /*   ---   FRIENDS   ---   */
    template <typename>
    friend class node_navigator;

    using node_ptrs_t = multiple_node_pointer<TargetNodePtr, GeneratedNodePtr>;

    /*   ---   ATTRIBUTES   ---   */
    NodeAllocator& allocator;

    /*   ---   CONSTRUCTORS   ---   */
    public:
    template <typename Pred = Predicate, typename = std::enable_if_t<std::is_default_constructible_v<Pred>>>
    generative_navigator() :
            node_pred_navigator<node_ptrs_t, Predicate>() {
    }

    template <typename OtherNodePtr, typename = std::enable_if_t<is_const_cast_equivalent<OtherNodePtr, TargetNodePtr>>>
    generative_navigator(
        const generative_navigator<NodeAllocator, Predicate, OtherNodePtr, GeneratedNodePtr>& other) :
            generative_navigator(static_cast<node_ptrs_t>(other.root, other.is_subtree)) {
    }

    generative_navigator(
        NodeAllocator& allocator,
        multiple_node_pointer<TargetNodePtr, GeneratedNodePtr> root,
        Predicate predicate,
        bool is_subtree = true) :
            node_pred_navigator<node_ptrs_t, Predicate>(root, predicate, is_subtree),
            allocator(allocator) {
    }

    /*   ---   ASSIGNMENT   ---   */
    template <typename OtherNodePtr, typename = std::enable_if_t<is_const_cast_equivalent<OtherNodePtr, TargetNodePtr>>>
    generative_navigator& operator=(
        const generative_navigator<NodeAllocator, Predicate, OtherNodePtr, GeneratedNodePtr>& other) {
        this->is_subtree = other.is_subtree;
        this->root       = static_cast<node_ptrs_t>(other.root);
    }

    /*   ---   METHODS   ---   */
    public:
    node_ptrs_t get_prev_sibling(node_ptrs_t node) const {
        assert(node.all_valid());
        node_ptrs_t result(this->node_pred_navigator<node_ptrs_t, Predicate>::get_prev_sibling(node));
        auto&& [target, generated] = result.get_pointers();
        if (target && !generated) {
            std::get<1>(node.get_pointers())
                ->get_parent()
                ->prepend_child(allocate(this->allocator, target->get_value()).release());
        }
        return result;
    }

    node_ptrs_t get_next_sibling(node_ptrs_t node) const {
        assert(node.all_valid());
        node_ptrs_t result(this->node_pred_navigator<node_ptrs_t, Predicate>::get_next_sibling(node));
        auto&& [target, generated] = result.get_pointers();
        if (target && !generated) {
            std::get<1>(node.get_pointers())
                ->get_parent()
                ->append_child(allocate(this->allocator, target->get_value()).release());
        }
        return result;
    }

    node_ptrs_t get_first_child(node_ptrs_t node) const {
        assert(node.all_valid());
        node_ptrs_t result(this->node_pred_navigator<node_ptrs_t, Predicate>::get_first_child(node));
        auto&& [target, generated] = result.get_pointers();
        if (target && !generated) {
            std::get<1>(node.get_pointers())
                ->prepend_child(allocate(this->allocator, target->get_value()).release());
        }
        return result;
    }

    node_ptrs_t get_last_child(node_ptrs_t node) const {
        assert(node.all_valid());
        node_ptrs_t result(this->node_pred_navigator<node_ptrs_t, Predicate>::get_last_child(node));
        auto&& [target, generated] = result.get_pointers();
        if (target && !generated) {
            std::get<1>(node.get_pointers())
                ->prepend_child(allocate(this->allocator, target->get_value()).release());
        }
        return result;
    }

    template <typename N = TargetNodePtr>
    node_ptrs_t get_left_child(N node) const {
        assert(node.all_valid());
        node_ptrs_t result(this->node_pred_navigator<node_ptrs_t, Predicate>::get_left_child(node));
        auto&& [target, generated] = result.get_pointers();
        if (target && !generated) {
            std::get<1>(node.get_pointers())
                ->prepend_child(allocate(this->allocator, target->get_value()).release());
        }
        return result;
    }

    template <typename N = TargetNodePtr>
    node_ptrs_t get_right_child(N node) const {
        assert(node.all_valid());
        node_ptrs_t result(this->node_pred_navigator<node_ptrs_t, Predicate>::get_right_child(node));
        auto&& [target, generated] = result.get_pointers();
        if (target && !generated) {
            std::get<1>(node.get_pointers())
                ->append_child(allocate(this->allocator, target->get_value()).release());
        }
        return result;
    }
};

} // namespace md
