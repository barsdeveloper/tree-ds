#pragma once

#include <type_traits>

#include <TreeDS/allocator_utility.hpp>
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
    using node_t      = std::remove_pointer_t<GeneratedNodePtr>;

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
    private:
    template <typename NavigateF, typename AttachF>
    node_ptrs_t do_navigate_generate(node_ptrs_t node, NavigateF&& navigate, AttachF&& attach_node) const {
        assert(node.all_valid());
        node_ptrs_t result(navigate(this, node));
        auto&& [target, generated] = result.get_pointers();
        if (target && !generated) {
            generated = attach_node(
                std::get<1>(node.get_pointers()),
                allocate(this->allocator, target->get_value()),
                *target);
        }
        return result;
    }

    public:
    node_ptrs_t get_prev_sibling(node_ptrs_t node) const {
        return this->do_navigate_generate(
            node,
            std::mem_fn(&node_pred_navigator<node_ptrs_t, Predicate>::get_prev_sibling),
            [](auto* target, unique_node_ptr<NodeAllocator> new_node, const auto& reference_node) {
                return target->get_parent()->assign_child_like(std::move(new_node), reference_node);
            });
    }

    node_ptrs_t get_next_sibling(node_ptrs_t node) const {
        return this->do_navigate_generate(
            node,
            std::mem_fn(&node_pred_navigator<node_ptrs_t, Predicate>::get_next_sibling),
            [](auto* target, unique_node_ptr<NodeAllocator> new_node, const auto& reference_node) {
                return target->get_parent()->assign_child_like(std::move(new_node), reference_node);
            });
    }

    node_ptrs_t get_first_child(node_ptrs_t node) const {
        return this->do_navigate_generate(
            node,
            std::mem_fn(&node_pred_navigator<node_ptrs_t, Predicate>::get_first_child),
            std::mem_fn(&node_t::template assign_child_like<NodeAllocator>));
    }

    node_ptrs_t get_last_child(node_ptrs_t node) const {
        return this->do_navigate_generate(
            node,
            std::mem_fn(&node_pred_navigator<node_ptrs_t, Predicate>::get_last_child),
            std::mem_fn(&node_t::template assign_child_like<NodeAllocator>));
    }

    template <typename N = TargetNodePtr>
    node_ptrs_t get_left_child(N node) const {
        return this->do_navigate_generate(
            node,
            std::mem_fn(&node_pred_navigator<node_ptrs_t, Predicate>::template get_left_child<N>),
            std::mem_fn(&node_t::template assign_child_like<NodeAllocator>));
    }

    template <typename N = TargetNodePtr>
    node_ptrs_t get_right_child(N node) const {
        return this->do_navigate_generate(
            node,
            std::mem_fn(&node_pred_navigator<node_ptrs_t, Predicate>::template get_right_child<N>),
            std::mem_fn(&node_t::template assign_child_like<NodeAllocator>));
    }
};

} // namespace md
