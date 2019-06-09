#pragma once

#include <cstddef> // std::size_t
#include <tuple>   // std::tuple
#include <type_traits>

#include <TreeDS/utility.hpp>

namespace md {

template <typename T, typename... Children>
class struct_node {

    template <typename, typename...>
    friend class struct_node;

    /*   ---   TYPES   ---   */
    public:
    using value_t = T;

    /*   ---   ATTRIBUTES   ---   */
    protected:
    value_t value;                       // Value hold by this node
    std::tuple<Children...> children {}; // Tuple containing actual children
    std::size_t subtree_size  = 1;       // Number of nodes of the tree considering this one as root
    std::size_t subtree_arity = 0;       // Arity of the tree having this node as root
    std::size_t index         = 0;       // The index of this node as a child

    /*   ---   CONSTRUCTORS   ---   */
    public:
    constexpr struct_node(const T& value, const Children&... children) :
            value(value),
            children(children...) {
        std::size_t size           = std::is_same_v<T, detail::empty_t> ? 0 : 1;
        std::size_t prev_arity     = 0;
        std::size_t children_count = 0;
        if constexpr (sizeof...(Children) > 0) {
            std::size_t index  = 0;
            auto do_each_child = [&](auto& node) {
                node.index = index++;
                if constexpr (std::is_same_v<decltype(node.get_value()), detail::empty_t>) {
                    return;
                } else {
                    ++children_count;
                }
                size += node.get_subtree_size();
                prev_arity = std::max(prev_arity, node.get_subtree_arity());
            };
            std::apply(
                [&](auto&... child) {
                    (..., do_each_child(child));
                },
                this->children);
        }
        this->subtree_size  = size;
        this->subtree_arity = std::max(children_count, prev_arity);
    }

    ~struct_node() = default;

    constexpr struct_node(const struct_node& other) = default;

    /*   ---   METHODS   ---   */
    static constexpr std::size_t children_count() {
        return sizeof...(Children);
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
    constexpr std::size_t get_index() const {
        return this->index;
    }

    template <typename... Nodes>
    constexpr struct_node<T, Nodes...> operator()(Nodes&&... nodes) const {
        return {this->value, nodes...};
    }

    constexpr const std::tuple<Children...>& get_children() const {
        return this->children;
    }
};

// Functions to generate struct_node objects
template <typename T>
constexpr struct_node<T> n(T value) {
    return {value};
}

template <typename... Args>
constexpr struct_node<std::tuple<Args...>> n(Args... args) {
    return {std::make_tuple(args...)};
}

constexpr struct_node<detail::empty_t> n() {
    return {detail::empty_t {}};
}

template <std::size_t index, typename T, typename... Children>
constexpr const auto& get_child(const struct_node<T, Children...>& node) {
    return std::get<index>(node.get_children());
}

} // namespace md
