#pragma once

#include <functional>
#include <type_traits> // std::enable_if, std::is_convertible, std::is_constructible, std::void_t
#include <utility>     // std::forward(), std::declval

namespace ds {

// forward declarations
template <typename>
class binary_node;

template <typename>
class nary_node;

#define CHECK_CONVERTIBLE(FROM, TO) typename = std::enable_if_t<std::is_convertible_v<FROM, TO>>
#define CHECK_CONSTRUCTIBLE(TYPE, ARGS) typename = std::enable_if_t<std::is_constructible_v<TYPE, ARGS>>
#define CHECK_COPIABLE(TYPE) typename = std::enable_if<std::is_copy_constructible_v<TYPE>>

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

template <typename Node>
const Node* prev_branch_sibling(const Node& from) {
    int relative_level             = 0;
    const Node* deepest_last_child = &from;
    const Node* left_crossed;
    do {
        // Climb up the tree until a node with a previous sibling is found, thne return that previous sibling
        left_crossed = keep_calling(
            // from
            *deepest_last_child,
            // keep calling
            std::mem_fn(&Node::get_parent),
            // until
            [&](const Node& child, const Node&) {
                --relative_level;
                return !child.is_first_child();
            },
            // then return
            [&](const Node& child, const Node&) {
                // because it considers the previous value (first argument)
                ++relative_level;
                // always present because it's the first child
                return child.get_prev_sibling();
            });
        if (left_crossed->is_root()) {
            return nullptr;
        } else if (relative_level == 0) {
            return left_crossed;
        }
        // Descend the tree following the last children until level is reached or no more children
        deepest_last_child = keep_calling(
            // from
            *left_crossed,
            // keep calling
            std::mem_fn(&Node::get_last_child),
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
    return deepest_last_child;
}

template <typename Node>
const Node* upper_row_rightmost(const Node& from) {
    if (from.is_root()) {
        return nullptr;
    }
    int relative_level = 0;
    const Node* deepest_last_child;
    // Climb the tree up to the root
    const Node* left_crossed = keep_calling(
        // from
        from,
        // keep calling
        std::mem_fn(&Node::get_parent),
        // until
        [&](const Node&, const Node& parent) {
            --relative_level;
            return parent.is_root();
        },
        // then return
        [](const Node&, const Node& root) {
            return &root;
        });
    // if it climbed just once, then this is a child of the root anche the root is its upper_row_rightmost's
    if (relative_level == -1) {
        return left_crossed;
    }
    do {
        // Descend the tree keeping right (last child)
        deepest_last_child = keep_calling(
            // from
            *left_crossed,
            // keep calling
            std::mem_fn(&Node::get_last_child),
            // until
            [&](const Node&, const Node&) {
                ++relative_level;
                return relative_level >= -1;
            },
            // the return
            [](const Node&, const Node& child) {
                return &child;
            });
        if (relative_level == -1) {
            return deepest_last_child;
        }
        // Go to the node that is on the left and at the same level
        left_crossed = prev_branch_sibling(*deepest_last_child);
    } while (relative_level < -1);
    return deepest_last_child;
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
            [](const Node&, const Node& child) {
                return &child;
            });
        if (current_level > deepest_level) {
            deepest_level = current_level;
            deepest_node  = current_node;
        }
        // Go to the node that is on the left and at the same level
        current_node = prev_branch_sibling(*current_node);
    } while (current_node != nullptr);
    return deepest_node;
}

/**
 * This is a type trait used to verify whheter a given traversal policy is updateable, i.e. if it has a method update
 * callable with a two arguments of type Arg. For a concrete example take a look at {@link breadth_first#update}.
 */
template <
    typename T,      // type to be cheacked
    typename Arg,    // arguments to be provided to method: "update"
    typename = void> // dummy argument to exclude this template overload from being taken into consideration
struct is_updateable : std::false_type {};

template <typename T, typename Arg>
struct is_updateable<
    T,   // type to be cheacked
    Arg, // arguments to be provided to method: "update"
    // the whole thing result in void (it is just to satisfy template parameter 3
    std::void_t<
        // unevaluated context
        decltype(
            // declare a value of type T (works also if T is not default constructible
            std::declval<T>()
                // call method update
                .update(
                    // provide it arguments Node
                    std::declval<const Arg&>(),
                    std::declval<const Arg&>()))>>
        : std::true_type {};

template <
    typename T,
    typename Tuple,
    typename = void>
struct is_constructible_from_tuple : std::false_type {};

template <
    typename T,
    typename Tuple>
struct is_constructible_from_tuple<
    T,
    Tuple,
    std::enable_if_t<
        std::is_invocable_v<
            std::make_from_tuple<T>,
            const Tuple&>>>
        : std::true_type {};

template <typename T>
std::size_t count_nodes(const binary_node<T>& node) {
    return 1
        + (node.get_left_child() != nullptr
               ? count_nodes(*node.get_left_child())
               : 0)
        + (node.get_right_child() != nullptr
               ? count_nodes(*node.get_right_child())
               : 0);
}

template <typename T>
std::size_t count_nodes(const nary_node<T>& node) {
    std::size_t size          = 1;
    const nary_node<T>* child = node.get_first_child();
    while (child != nullptr) {
        size += count_nodes(*child);
        child = child->get_next_sibling();
    }
    return size;
}

} // namespace ds
