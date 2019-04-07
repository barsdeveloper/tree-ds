#pragma once

#include <cstddef>     // std::ptr_diff_t
#include <functional>  // std::mem_fn()
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
Node* keep_calling(Node& from, Call call, Test test, Result result) {
    Node* prev = &from;
    Node* next = call(*prev);
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
Node* keep_calling(Node& from, Callable call) {
    Node* prev = &from;
    Node* next = call(*prev);
    while (next != nullptr) {
        prev = next;
        next = call(*prev);
    }
    return prev;
}

/*
 * Confused by the purpose of std::decay_t as a wrapper for the second parameter? It is worth spending a few words to
 * clarify this awesome trick. To start with, let's understand what we wanted to achieve here:
 *   - Arguments and return type are variations of some basic template Node.
 *   - Returned type must preserve the constness of the first argument.
 *   - The second argument must accept any Node (costant/non-costant).
 *
 * This means that we can have:
 *     `Node m;`
 *     `const Node c;`
 *
 * Then all the following calls must be valid:
 *     `calculate_other_branch_node(m, m); // (1) returns a pointer to mutable`
 *     `calculate_other_branch_node(m, c); // (2) returns a pointer to mutable`
 *     `calculate_other_branch_node(c, m); // (3) returns a pointer to const`
 *     `calculate_other_branch_node(c, c); // (4) returns a pointer to const`
 *
 * Why this, more intuitive, implementation fails?
 *     template <typename Node, bool Left>
 *     Node* calculate_other_branch_node(Node& from, const Node& root);
 *
 * Among all the 4 possibilities above, just (1) and (2) compile: Node is deduced node_t:
 *     calculate_other_branch_node(node_t&, const node_t&);
 * In the other cases the argument cannot be deduced.
 *
 * The solution is to "mark" somehow the arguments that do no partecipate in the template parameter deduction. In
 * C++ standardese parlance this translates into "Non-deduced context":
 * https://en.cppreference.com/w/cpp/language/template_argument_deduction#Non-deduced_contexts
 * The case we are reproducing is the first: nested-name-specifier, here `std::decay_t` comes into play. This type trait
 * just removes qualifications from a type: reference, const, volative... Intuitively it will give a "basic" version of
 * a type. This way, for example, `const int` and `int&&` will give `int`, while `int[]` will give `int*`.
 * `std::decay_t<T>` is actually the typedef `std::decay<T>::type`
 *                                                           ^^^^ dependand type
 * With this trick, we just took out of the picture the last parameter in the determination of the template parameter
 * type. In fact C++20 has a new trait used to exploit this mechanism: `std::type_identity`. In this specific scenario,
 * decay covers our needs pretty well.
 */
template <typename Node, bool Left>
Node* calculate_other_branch_node(Node& from, const std::decay_t<Node>& root) {
    int relative_level  = 0;
    Node* deepest_child = &from;
    Node* crossed;
    do {
        // Climb up the tree until a node with a sibling is found, then return that sibling
        crossed = keep_calling(
            // from
            *deepest_child,
            // keep calling
            [&](Node& node) {
                return node.get_parent_limit(root);
            },
            // until
            [&](Node& child, Node&) {
                --relative_level;
                return Left
                    ? !child.is_first_child()
                    : !child.is_last_child();
            },
            // then return
            [&](Node& child, Node&) {
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
            [](Node& node) {
                return Left
                    ? node.get_last_child()
                    : node.get_first_child();
            },
            // until
            [&](Node&, Node&) {
                ++relative_level;
                return relative_level >= 0;
            },
            // the return
            [](Node&, Node& child) {
                return &child;
            });
    } while (relative_level < 0);
    return deepest_child;
} // namespace md

template <typename Node>
Node* left_branch_node(Node& from, const std::decay_t<Node>& root) {
    return calculate_other_branch_node<Node, true>(from, root);
}

template <typename Node>
Node* right_branch_node(Node& from, const std::decay_t<Node>& root) {
    return calculate_other_branch_node<Node, false>(from, root);
}

template <typename Node, bool Left>
Node* calculate_row_extremum(Node& from, const std::decay_t<Node>& root) {
    if (from.is_root_limit(root)) {
        return &from;
    }
    int relative_level = 0;
    Node* deepest_extremum_child;
    // Climb the tree up to the root
    Node* breanch_crossed = keep_calling(
        // from
        from,
        // keep calling
        [](Node& node) {
            return node.get_parent();
        },
        // until
        [&](Node&, Node& parent) {
            --relative_level;
            return parent.is_root_limit(root);
        },
        // then return
        [](Node&, Node& root) {
            return &root;
        });
    do {
        // Descend the tree traversing extremum children
        deepest_extremum_child = keep_calling(
            // from
            *breanch_crossed,
            // keep calling
            [](Node& node) {
                return Left
                    ? node.get_first_child()
                    : node.get_last_child();
            },
            // until
            [&](Node&, Node&) {
                ++relative_level;
                return relative_level >= 0;
            },
            // the return
            [](Node&, Node& child) {
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
Node* same_row_leftmost(Node& from, const std::decay_t<Node>& root) {
    return calculate_row_extremum<Node, true>(from, root);
}

template <typename Node>
Node* same_row_rightmost(Node& from, const std::decay_t<Node>& root) {
    return calculate_row_extremum<Node, false>(from, root);
}

template <typename Node>
Node* deepest_rightmost_child(Node& root) {
    int current_level  = 0;
    int deepest_level  = 0;
    Node* current_node = &root;
    Node* deepest_node = &root;
    do {
        // Descend the tree keeping right (last child)
        current_node = keep_calling(
            // from
            *current_node,
            // keep calling
            [](Node& node) {
                return node.get_last_child();
            },
            // until
            [&](Node&, Node&) {
                ++current_level;
                return false;
            },
            // the return
            std::function<Node*(Node&, Node&)>()); // this is never called ("until" lambda retuns always false)
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
std::size_t calculate_arity(const Node& node, std::size_t max_expected_arity) {
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

// Check if two types are instantiation of the same template
template <typename, typename>
constexpr bool is_same_template = false;

template <
    template <typename...> class T,
    typename... A,
    typename... B>
constexpr bool is_same_template<T<A...>, T<B...>> = true;

} // namespace md
