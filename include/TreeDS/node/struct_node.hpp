#pragma once

#include <cstddef> // std::size_t
#include <tuple>   // std::tuple
#include <type_traits>

namespace md {

template <typename T, typename... Children>
class struct_node {

    //   ---   TYPES   ---
    public:
    using children_t = std::tuple<Children...>;
    using value_t    = T;

    //   ---   ATTRIBUTES   ---
    protected:
    value_t value;                 // Value hold by this node
    std::size_t subtree_size  = 1; // Number of nodes of the tree considering this one as root.
    std::size_t subtree_arity = 0; // Arity of the tree having this node as root.
    children_t children{};         // Tuple containing actual children

    //   ---   CONSTRUCTORS   ---
    public:
    constexpr struct_node(const T& value, Children&&... children) :
            value(value),
            children(std::forward<Children>(children)...) {
        std::size_t size           = std::is_same_v<T, std::nullptr_t> ? 0 : 1;
        std::size_t prev_arity     = 0;
        std::size_t children_count = 0;
        if constexpr (sizeof...(Children) > 0) {
            auto call = [&](auto& node) {
                if constexpr (std::is_same_v<decltype(node.get_value()), std::nullptr_t>) {
                    return;
                } else {
                    children_count += 1;
                }
                size += node.get_subtree_size();
                prev_arity = std::max(prev_arity, node.get_subtree_arity());
            };
            (call(children), ...);
        }
        this->subtree_size  = size;
        this->subtree_arity = std::max(children_count, prev_arity);
    }

    ~struct_node() = default;

    constexpr struct_node(const struct_node& other) = default;

    //   ---   METHODS   ---
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

    constexpr const children_t& get_children() const {
        return this->children;
    }
};

// Functions to generate temporary_node objects.
template <typename T>
constexpr struct_node<T> n(T value) {
    return {value};
}

template <typename... Args>
constexpr struct_node<std::tuple<Args...>> n(Args... args) {
    return {std::make_tuple(args...)};
}

constexpr struct_node<std::nullptr_t> n() {
    return {nullptr};
}

template <std::size_t index, typename T, typename... Children>
constexpr const auto& get_child(const struct_node<T, Children...>& node) {
    return std::get<index>(node.get_children());
}

} // namespace md
