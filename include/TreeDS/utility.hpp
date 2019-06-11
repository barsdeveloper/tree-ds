#pragma once

#include <cassert>    // assert
#include <cstddef>    // std::size_t
#include <functional> // std::invoke()
#include <iostream>   // std::cout
#include <string>
#include <string_view>
#include <tuple>       // std::std::make_from_tuple
#include <type_traits> // std::std::enable_if_t, std::is_invocable_v, std::void_t
#include <utility>     // std::declval(), std::make_index_sequence

namespace md {

/*   ---   FORWARD DECLARATIONS   ---   */
template <typename>
class binary_node;

template <typename>
class nary_node;

template <typename>
class node_navigator;

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

template <typename Policy, typename = void>
constexpr bool is_tag_of_policy = false;

template <typename Policy>
constexpr bool is_tag_of_policy<
    Policy,
    std::void_t<
        decltype(
            std::declval<Policy>()
                .template get_instance<
                    binary_node<int>*,
                    node_navigator<binary_node<int>*>,
                    std::allocator<binary_node<int>>>(
                    std::declval<binary_node<int>*>(),
                    std::declval<node_navigator<binary_node<int>*>>(),
                    std::declval<std::allocator<int>>())),
        Policy>> = true;

// Check method Type::get_resources() exists
template <typename Type, typename = void>
constexpr bool holds_resources = false;

template <typename Type>
constexpr bool holds_resources<
    Type,
    std::void_t<decltype(std::declval<Type>().get_resources())>> = true;

// Check if two types are instantiation of the same template
template <typename T, typename U>
constexpr bool is_same_template = std::is_same_v<T, U>;

template <
    template <typename...> class T,
    typename... A,
    typename... B>
constexpr bool is_same_template<T<A...>, T<B...>> = true;

template <
    template <auto...> class T,
    auto... A,
    auto... B>
constexpr bool is_same_template<T<A...>, T<B...>> = true;

template <typename A, typename B>
constexpr bool is_const_cast_equivalent
    = std::conjunction_v<
        // If both are either pointers or not
        std::bool_constant<std::is_pointer_v<A> == std::is_pointer_v<B>>,
        // They point to the same type (ignoring const/volatile)
        std::is_same<
            std::decay_t<std::remove_pointer_t<A>>,
            std::decay_t<std::remove_pointer_t<B>>>>;

template <typename T, typename = void>
constexpr bool is_printable = false;

template <typename T>
constexpr bool is_printable<T, std::void_t<decltype(std::declval<decltype(std::cout)>() << std::declval<T>())>> = true;

namespace detail {
    struct empty_t {};
} // namespace detail

template <typename Node, typename Call, typename Test, typename Result>
Node keep_calling(Node from, Call&& call, Test&& test, Result&& result) {
    Node prev = from;
    Node next = call(prev);
    while (next != nullptr) {
        if (test(prev, next)) {
            return result(prev, next);
        }
        prev = next;
        next = call(prev);
    }
    return prev; // Returns something only if test() succeeds
}

/**
 * This function can be used to keep calling a specific lambda {@link Callable}. The passed type must be convertible to
 * a function that take a reference to constant node and returns a pointer to constant node. The best is to pass a
 * lambda so that it can be inlined.
 *
 * The case from == nullptr is correctly managed.
 */
template <typename Node, typename Call>
Node keep_calling(Node from, Call&& call) {
    Node prev = from;
    Node next = call(prev);
    while (next != nullptr) {
        prev = next;
        next = call(prev);
    }
    return prev;
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

namespace detail {
    /***
     * @brief Retrieve unsing a runtime index the element of a tuple.
     */
    template <std::size_t Target> // target is index + 1, Target = 0 is invalid
    struct element_apply_construction {
        template <typename F, typename Tuple>
        static constexpr decltype(auto) single_apply(F&& f, Tuple&& tuple, std::size_t index) {
            static_assert(Target > 0, "Requested index does not exist in the tuple.");
            if (index == Target - 1) {
                return f(std::get<Target - 1>(tuple));
            } else {
                return element_apply_construction<Target - 1>::single_apply(
                    std::forward<F>(f),
                    std::forward<Tuple>(tuple),
                    index);
            }
        }
    };
    template <>
    struct element_apply_construction<0> {
        template <typename F, typename Tuple>
        static constexpr decltype(auto) single_apply(F&& f, Tuple&& tuple, std::size_t) {
            assert(false);
            // This return is just for return type deduction
            return f(std::get<0>(tuple));
        }
    };
} // namespace detail

/**
 * @brief Invoke the Callable object with an argument taken from the tuple at position {@link index}.
 */
template <typename F, typename Tuple>
decltype(auto) apply_at_index(F&& f, Tuple&& tuple, std::size_t index) {
    static_assert(is_same_template<std::decay_t<Tuple>, std::tuple<>>, "Expected the second argument to be a tuple.");
    constexpr std::size_t tuple_size = std::tuple_size_v<std::decay_t<Tuple>>;
    return detail::element_apply_construction<tuple_size>::single_apply(
        std::forward<F>(f),
        std::forward<Tuple>(tuple),
        index);
}

void code_like_print(std::ostream& stream) {
    stream << "n()";
}

void code_like_print(std::ostream& stream, char c) {
    stream << '\'' << c << '\'';
}

void code_like_print(std::ostream& stream, const char* c) {
    stream << '\"' << c << '\"';
}

void code_like_print(std::ostream& stream, const std::string& c) {
    stream << '\"' << c << '\"';
}

void code_like_print(std::ostream& stream, std::string_view c) {
    stream << '\"' << c << '\"';
}

template <typename T>
void code_like_print(std::ostream& stream, const T& c) {
    stream << c;
}

struct print_preferences {
    unsigned indentation_increment = 4;
    int limit                      = 10;
};

template <typename Node>
void print_node(
    std::ostream& os,
    const Node& node,
    unsigned indentation,
    print_preferences&& preferences = {}) {
    if (preferences.limit <= 0) {
        return;
    }
    os << std::string(indentation, ' ') << "n(";
    code_like_print(os, node.get_value());
    os << ')';
    const Node* current = node.get_first_child();
    if (current) {
        os << "(\n";
        if (--preferences.limit > 0) {
            print_node(os, *current, indentation + preferences.indentation_increment, std::move(preferences));
            for (
                current = current->get_next_sibling();
                current != nullptr;
                current = current->get_next_sibling()) {
                os << ",\n";
                if (--preferences.limit > 0) {
                    print_node(os, *current, indentation + preferences.indentation_increment, std::move(preferences));
                } else {
                    os << std::string(indentation + preferences.indentation_increment, ' ') << "...";
                    break;
                }
            }
        } else {
            os << std::string(indentation + 4, ' ') << "...";
        }
        os << ")";
    }
}

} // namespace md
