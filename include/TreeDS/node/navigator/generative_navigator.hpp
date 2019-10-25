#pragma once

#include <type_traits>

#include <TreeDS/allocator_utility.hpp>
#include <TreeDS/node/multiple_node_pointer.hpp>
#include <TreeDS/node/navigator/node_pred_navigator.hpp>
#include <TreeDS/utility.hpp>

namespace md {

template <typename TargetNodePtr, typename GeneratedNodePtr, typename Predicate, typename NodeAllocator>
class generative_navigator
        : public node_pred_navigator<
              multiple_node_pointer<TargetNodePtr, GeneratedNodePtr>,
              Predicate> {

    /*   ---   TYPES   ---   */
    public:
    using generated_node_type = std::remove_pointer_t<GeneratedNodePtr>;
    using node_ptrs_t         = multiple_node_pointer<TargetNodePtr, GeneratedNodePtr>;
    using typename node_pred_navigator<node_ptrs_t, Predicate>::node_pointer;
    using typename node_pred_navigator<node_ptrs_t, Predicate>::node_type;

    /*   ---   ATTRIBUTES   ---   */
    NodeAllocator allocator;

    /*   ---   CONSTRUCTORS   ---   */
    public:
    generative_navigator() {
    }

    template <typename OtherNodePtr, typename = std::enable_if_t<std::is_convertible_v<OtherNodePtr, TargetNodePtr>>>
    generative_navigator(const generative_navigator<OtherNodePtr, GeneratedNodePtr, Predicate, NodeAllocator>& other) :
            node_pred_navigator<node_ptrs_t, Predicate>(other),
            allocator(other.allocator) {
    }

    template <typename OtherNodePtr, typename = std::enable_if_t<std::is_convertible_v<OtherNodePtr, TargetNodePtr>>>
    generative_navigator(
        multiple_node_pointer<OtherNodePtr, GeneratedNodePtr> root,
        Predicate predicate,
        NodeAllocator& allocator) :
            node_pred_navigator<node_ptrs_t, Predicate>(root, predicate),
            allocator(allocator) {
    }

    /*   ---   ASSIGNMENT   ---   */
    template <typename OtherNodePtr, typename = std::enable_if_t<is_const_cast_equivalent<OtherNodePtr, TargetNodePtr>>>
    generative_navigator& operator=(
        const generative_navigator<Predicate, OtherNodePtr, GeneratedNodePtr, NodeAllocator>& other) {
        this->node_pred_navigator<node_ptrs_t, Predicate>::operator=(other);
    }

    /*   ---   METHODS   ---   */
    private:
    template <typename NavigateF, typename AttachF>
    node_ptrs_t do_navigate_generate(node_ptrs_t& node, NavigateF&& navigate, AttachF&& attach_node) {
        assert(node.all_valid());
        node_ptrs_t result(navigate(this, node));
        auto&& [reference_node, generated] = result.get_pointers();
        unique_ptr_alloc<NodeAllocator> assigned_child;
        if (reference_node) {
            if (!generated) {
                result.assign_pointer(1, allocate(this->allocator, reference_node->get_value()).release());
            }
            if (result.is_pointer_assigned(1)) {
                attach_node(
                    std::get<1>(node.get_pointers()),
                    unique_ptr_alloc<NodeAllocator>(generated),
                    *reference_node);
            }
        }
        return result;
    }

    public:
    node_ptrs_t get_prev_sibling(node_ptrs_t node) {
        return this->do_navigate_generate(
            node,
            std::mem_fn(&node_pred_navigator<node_ptrs_t, Predicate>::get_prev_sibling),
            [](auto* generated, unique_ptr_alloc<NodeAllocator> new_node, const auto& reference_node) {
                return generated->get_parent()->assign_child_like(std::move(new_node), reference_node);
            });
    }

    node_ptrs_t get_next_sibling(node_ptrs_t node) {
        return this->do_navigate_generate(
            node,
            std::mem_fn(&node_pred_navigator<node_ptrs_t, Predicate>::get_next_sibling),
            [](auto* generated, unique_ptr_alloc<NodeAllocator> new_node, const auto& reference_node) {
                return generated->get_parent()->assign_child_like(std::move(new_node), reference_node);
            });
    }

    node_ptrs_t get_first_child(node_ptrs_t node) {
        return this->do_navigate_generate(
            node,
            std::mem_fn(&node_pred_navigator<node_ptrs_t, Predicate>::get_first_child),
            std::mem_fn(&generated_node_type::template assign_child_like<NodeAllocator>));
    }

    node_ptrs_t get_last_child(node_ptrs_t node) {
        return this->do_navigate_generate(
            node,
            std::mem_fn(&node_pred_navigator<node_ptrs_t, Predicate>::get_last_child),
            std::mem_fn(&generated_node_type::template assign_child_like<NodeAllocator>));
    }

    template <
        typename N = TargetNodePtr,
        typename   = std::enable_if_t<is_binary_node_pointer<N>>>
    node_ptrs_t get_left_child(N node) {
        return this->do_navigate_generate(
            node,
            std::mem_fn(&node_pred_navigator<node_ptrs_t, Predicate>::template get_left_child<N>),
            std::mem_fn(&generated_node_type::template assign_child_like<NodeAllocator>));
    }

    template <
        typename N = TargetNodePtr,
        typename   = std::enable_if_t<is_binary_node_pointer<N>>>
    node_ptrs_t get_right_child(N node) {
        return this->do_navigate_generate(
            node,
            std::mem_fn(&node_pred_navigator<node_ptrs_t, Predicate>::template get_right_child<N>),
            std::mem_fn(&generated_node_type::template assign_child_like<NodeAllocator>));
    }
};

template <typename TargetNodePtr, typename GeneratedNodePtr, typename Predicate, typename Allocator>
generative_navigator(multiple_node_pointer<TargetNodePtr, GeneratedNodePtr>, Predicate, Allocator)
    ->generative_navigator<TargetNodePtr, GeneratedNodePtr, Predicate, Allocator>;

} // namespace md
