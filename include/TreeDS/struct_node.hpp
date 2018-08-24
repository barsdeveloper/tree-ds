#pragma once

#include <cstddef> // std::size_t
#include <tuple>   // std::tuple

namespace ds {

struct empty {};

template <typename, typename...>
class struct_node;

template <typename T>
struct_node<T> n(T);

struct_node<empty> n();

template <typename T, typename... Children>
class struct_node {

    //   ---   FRIENDS   ---
    template <typename, typename...>
    friend class struct_node;      // other instantiations of this template
    friend struct_node<T> n<T>(T); // node creator function
    friend struct_node<empty> n(); // empty node creator function

    //   ---   TYPES   ---
    public:
    using children_t = std::tuple<Children...>;
    using value_t    = T;

    //   ---   ATTRIBUTES   ---
    value_t value;
    std::size_t subtree_size  = 1; // Number of nodes of the tree considering this one as root.
    std::size_t subtree_arity = 0; // Arity of the tree having this node as root.
    children_t children;

    //   ---   METHODS   ---
    private:
    // Constructors are private because the instances must be constructed using function n().
    constexpr struct_node(const T& value, Children... children) :
            value(value),
            children(children...) {
        std::size_t size           = std::is_same_v<T, empty> ? 0 : 1;
        std::size_t prev_arity     = 0;
        std::size_t children_count = 0;
        if constexpr (sizeof...(Children) > 0) {
            auto call = [&](auto& node) {
                if constexpr (
                    std::is_same_v<
                        typename std::remove_reference_t<decltype(node)>::value_t,
                        empty>) {
                    return;
                }
                size += node.get_subtree_size();
                prev_arity = std::max(prev_arity, node.get_subtree_arity());
                children_count
                    += std::is_same_v<
                           typename std::remove_reference_t<decltype(node)>::value_t,
                           empty>
                    ? 0
                    : 1;
            };
            // If you don't understand this, check: https://en.cppreference.com/w/cpp/language/fold
            (call(children), ...);
        }
        this->subtree_size  = size;
        this->subtree_arity = std::max(children_count, prev_arity);
    }

    public:
    ~struct_node() = default;

    constexpr struct_node(const struct_node& other) :
            value(other.value),
            subtree_size(other.subtree_size),
            subtree_arity(other.subtree_arity),
            children(other.children) {
    }

    constexpr bool has_children() const {
        return this->children_count() > 0;
    }

    constexpr std::size_t children_count() const {
        return std::tuple_size_v<children_t>;
    }

    constexpr T get_value() const {
        return this->value;
    }

    constexpr std::size_t get_subtree_size() const {
        return this->subtree_size;
    }

    constexpr std::size_t get_subtree_arity() const {
        return this->subtree_arity;
    }

    template <typename... Nodes>
    constexpr struct_node<T, Nodes...> operator()(Nodes&&... nodes) const {
        return {this->value, std::forward<Nodes>(nodes)...};
    }
};

// Functions to generate temporary_node objects.
template <typename T>
struct_node<T> n(T value) {
    return {value};
}

template <typename... Args>
struct_node<std::tuple<Args...>> n(Args... args) {
    return {std::make_tuple(args...)};
}

struct_node<empty> n() {
    return {empty()};
}

template <std::size_t index, typename T, typename... Children>
const auto& get_child(const struct_node<T, Children...>& node) {
    return std::get<index>(node.children);
}

} // namespace ds
