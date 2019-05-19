#pragma once

#include <cstddef> // std::size_t

#include <TreeDS/utility.hpp>

namespace md {

/*   ---   FORWARD DECLARATIONS   ---   */
template <typename, typename, typename...>
class matcher;

template <typename, typename>
class capture_node;

/*   ---   CLASSES DEINITIONS   ---   */
struct matcher_info_t {
    bool matches_null;
    bool reluctant;
    constexpr matcher_info_t(bool matches_null, bool reluctant) :
            matches_null(matches_null),
            reluctant(reluctant) {
    }
};

template <std::size_t Index>
struct capture_index {};

template <char... Name>
struct capture_name {};

namespace detail {
    /*   ---   METAPROGRAMMING VARIABLES   ---   */
    template <typename T>
    constexpr bool is_capture_name = is_same_template<T, capture_name<>> && !std::is_same_v<T, capture_name<>>;

    /*
     * This following variables are used to get the index of the matcher having a specific capture_name<> from captures
     * tuple. The correct way to use it is to always refer to index_of_capture<capture_name<...>, captures_t>. Let's see
     * it in action using an example.
     *
     * Examples (types are unrelated, just to keep notation simple and explain the logic behind):
     * Exists:
     * Name = int,
     * Tuple = std::tuple<std::vector<char>, std::list<int>, std::set<int>, std::deque<bool>>
     * We want to get the type set<int>
     * index_of_capture<int, std::tuple<std::vector<char>, std::list<int>, std::set<int>, std::deque<bool>>>
     *     = 1 + index_of_capture<int, std::tuple<std::list<int>, std::set<int>, std::deque<bool>>> // #3
     *     = 1 + 1 + index_of_capture<int, std::tuple<std::set<int>, std::deque<bool>>>             // #2
     *                                                ^^^^^^^^^^^^^ // Remember we are looking for this guy.
     *     = 1 + 1 + 0
     *     = 2 <- std::set<int> is Tuple's element 2.
     *
     * Doesn't exist:
     * Name = int,
     * index_of_capture<int, std::tuple<std::vector<char>, std::list<int>, std::set<int>, std::deque<bool>>>
     * We want to get the type deque<int>
     * index_of_capture<int, std::tuple<std::vector<char>, std::list<int>, std::set<int>, std::deque<bool>>>
     *     = 1 + index_of_capture<int, std::tuple<std::list<int>, std::set<int>, std::deque<bool>>> // #3
     *     = 1 + 1 + index_of_capture<int, std::tuple<std::set<int>, std::deque<bool>>>             // #3
     *     = 1 + 1 + 1 + index_of_capture<int, std::tuple<std::deque<bool>>>                        // #3
     *     = 1 + 1 + 1 + 1 + index_of_capture<int, std::tuple<>>                                    // #1
     *     = 1 + 1 + 1 + 1 + 0
     *     = 4 <- Larger than the maximum allowed index.
     */
    // 1
    template <
        typename Name,  // Name to look for.
        typename Tuple> // Tuple with the rest of the elements.
    constexpr std::size_t index_of_capture = 1;

    // 2
    template <typename CaptureName, typename Child, typename... Types>
    constexpr std::size_t index_of_capture<
        CaptureName,
        std::tuple<matcher<capture_node<CaptureName, Child>, CaptureName, Child>&, Types...>> = 0;
    //                                  ^^^^^^^^^^^          ^^^^^^^^^^^ We are looking for this

    // 3
    template <class T, class U, typename... Types>
    // Unpacking happens here: we discard the first element of the tuple (U) and recur using the remaining elements.
    constexpr std::size_t index_of_capture<
        T,
        std::tuple<U, Types...>> = 1 + index_of_capture<T, std::tuple<Types...>>;

    template <typename Name, typename Tuple>
    constexpr bool is_valid_name = index_of_capture<Name, Tuple> < std::tuple_size_v<Tuple>;
} // namespace detail

} // namespace md
