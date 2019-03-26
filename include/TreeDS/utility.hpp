#pragma once

#include <cstddef>     // std::ptr_diff_t
#include <functional>  // std::mem_fn()
#include <limits>      //std::numeric_limits
#include <type_traits> // std::enable_if, std::is_convertible, std::is_constructible, std::void_t
#include <utility>     // std::forward(), std::declval

namespace md {

// forward declarations
template <typename>
class binary_node;

template <typename>
class nary_node;

#define CHECK_CONVERTIBLE(FROM, TO) typename = std::enable_if_t<std::is_convertible_v<FROM, TO>>
#define CHECK_CONSTRUCTIBLE(TYPE, ARGS) typename = std::enable_if_t<std::is_constructible_v<TYPE, ARGS>>
#define CHECK_COPIABLE(TYPE) typename = std::enable_if_t<std::is_copy_constructible_v<TYPE>>

template <typename Node, typename Call, typename Test, typename Result>
const Node* keep_calling(const Node& from, Call call, Test test, Result result) {
    const Node* prev = &from;
    const Node* next = call(*prev);
    while (next != nullptr) {
        if (test(*prev, *next)) {
            return result(*prev, *next);
        }
        prev = next;
        next = call(*prev);
    }
    return prev; // returns something only if test() succeeds
}

/**
 * This function can be used to keep calling a specific lambda {@link Callable}. The passed type must be convertible to
 * a function that take a reference to constant node and returns a pointer to constant node. The best is to pass a
 * lambda so that it can be inlined.
 *
 * The case from == nullptr is correctly managed.
 */
template <typename Node, typename Callable>
const Node* keep_calling(const Node& from, Callable call) {
    const Node* prev = &from;
    const Node* next = call(*prev);
    while (next != nullptr) {
        prev = next;
        next = call(*prev);
    }
    return prev;
}

template <typename Node, bool Left>
const Node* calculate_other_branch_node(const Node& from, const Node& root) {
    int relative_level        = 0;
    const Node* deepest_child = &from;
    const Node* crossed;
    do {
        // Climb up the tree until a node with a sibling is found, then return that sibling
        crossed = keep_calling(
            // from
            *deepest_child,
            // keep calling
            [&](const Node& node) {
                return node.get_parent_limit(root);
            },
            // until
            [&](const Node& child, const Node&) {
                --relative_level;
                return Left
                    ? !child.is_first_child()
                    : !child.is_last_child();
            },
            // then return
            [&](const Node& child, const Node&) {
                // increment level because it considers the previous value (first argument)
                ++relative_level;
                return Left
                    // always present because it is not the first child
                    ? child.get_prev_sibling()
                    // always present because it is not the last child
                    : child.get_next_sibling();
            });
        if (crossed->is_root_limit(root)) {
            return nullptr;
        } else if (relative_level == 0) {
            return crossed;
        }
        // Descend the tree following the last children until level is reached or no more children
        deepest_child = keep_calling(
            // from
            *crossed,
            // keep calling
            std::mem_fn(
                Left
                    ? &Node::get_last_child
                    : &Node::get_first_child),
            // until
            [&](const Node&, const Node&) {
                ++relative_level;
                return relative_level >= 0;
            },
            // the return
            [](const Node&, const Node& child) {
                return &child;
            });
    } while (relative_level < 0);
    return deepest_child;
} // namespace md

template <typename Node>
const Node* left_branch_node(const Node& from, const Node& root) {
    return calculate_other_branch_node<Node, true>(from, root);
}

template <typename Node>
const Node* right_branch_node(const Node& from, const Node& root) {
    return calculate_other_branch_node<Node, false>(from, root);
}

template <typename Node, bool Left>
const Node* calculate_row_extremum(const Node& from, const Node& root) {
    if (from.is_root_limit(root)) {
        return &from;
    }
    int relative_level = 0;
    const Node* deepest_extremum_child;
    // Climb the tree up to the root
    const Node* breanch_crossed = keep_calling(
        // from
        from,
        // keep calling
        std::mem_fn(&Node::get_parent),
        // until
        [&](const Node&, const Node& parent) {
            --relative_level;
            return parent.is_root_limit(root);
        },
        // then return
        [](const Node&, const Node& root) {
            return &root;
        });
    do {
        // Descend the tree traversing extremum children
        deepest_extremum_child = keep_calling(
            // from
            *breanch_crossed,
            // keep calling
            std::mem_fn(
                Left
                    ? &Node::get_first_child
                    : &Node::get_last_child),
            // until
            [&](const Node&, const Node&) {
                ++relative_level;
                return relative_level >= 0;
            },
            // the return
            [](const Node&, const Node& child) {
                return &child;
            });
        if (relative_level == 0) {
            break;
        }
        // Go to the node that is on the other direction branch and at the same level
        breanch_crossed = calculate_other_branch_node<Node, !Left>(*deepest_extremum_child, root);
    } while (relative_level < 0);
    return deepest_extremum_child;
}

template <typename Node>
const Node* same_row_leftmost(const Node& from, const Node& root) {
    return calculate_row_extremum<Node, true>(from, root);
}

template <typename Node>
const Node* same_row_rightmost(const Node& from, const Node& root) {
    return calculate_row_extremum<Node, false>(from, root);
}

template <typename Node>
const Node* deepest_rightmost_child(const Node& root) {
    int current_level        = 0;
    int deepest_level        = 0;
    const Node* current_node = &root;
    const Node* deepest_node = &root;
    do {
        // Descend the tree keeping right (last child)
        current_node = keep_calling(
            // from
            *current_node,
            // keep calling
            std::mem_fn(&Node::get_last_child),
            // until
            [&](const Node&, const Node&) {
                ++current_level;
                return false;
            },
            // the return
            std::function<Node*(const Node&, const Node&)>()); // this is never called (until retuns always false)
        if (current_level > deepest_level) {
            deepest_level = current_level;
            deepest_node  = current_node;
        }
        // Go to the node that is on the left and at the same level
        current_node = left_branch_node(*current_node, root);
    } while (current_node != nullptr);
    return deepest_node;
}

template <typename T>
std::size_t calculate_size(const binary_node<T>& node) {
    return 1
        + (node.get_left_child() != nullptr
               ? calculate_size(*node.get_left_child())
               : 0)
        + (node.get_right_child() != nullptr
               ? calculate_size(*node.get_right_child())
               : 0);
}

template <typename T>
std::size_t calculate_size(const nary_node<T>& node) {
    std::size_t size          = 1;
    const nary_node<T>* child = node.get_first_child();
    while (child != nullptr) {
        size += calculate_size(*child);
        child = child->get_next_sibling();
    }
    return size;
}

template <typename Node>
std::size_t calculate_arity(const Node& node, std::size_t max_expected_arity = std::numeric_limits<std::size_t>::max()) {
    const Node* child = node.get_first_child();
    std::size_t arity = child
        ? child->get_following_siblings() + 1
        : 0u;
    while (child) {
        if (arity == max_expected_arity) {
            return arity;
        }
        arity = std::max(arity, calculate_arity(*child, max_expected_arity));
        child = child->get_next_sibling();
    }
    return arity;
}

template <
    typename T,
    typename Tuple,
    typename = void>
constexpr bool is_constructible_from_tuple = false;

template <
    typename T,
    typename Tuple>
constexpr bool is_constructible_from_tuple<
    T,
    Tuple,
    std::enable_if_t<
        std::is_invocable_v<
            std::make_from_tuple<T>,
            const Tuple&>>> = true;

template <typename Policy, typename Node, typename Allocator, typename = void>
constexpr bool is_tag_of_policy = false;

template <typename Policy, typename Node, typename Allocator>
constexpr bool is_tag_of_policy<
    Policy,
    Node,
    Allocator,
    std::void_t<
        decltype(
            std::declval<Policy>().template get_instance<Node, Allocator>(
                std::declval<Node*>(),
                std::declval<Node*>(),
                std::declval<Allocator>())),
        Policy>> = true;

// Check method Type::get_resources() exists
template <typename Type, typename = void>
constexpr bool holds_resources = false;

template <typename Type>
constexpr bool holds_resources<
    Type,
    std::void_t<decltype(std::declval<Type>().get_resources())>> = true;

} // namespace md
