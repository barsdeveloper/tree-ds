#pragma once

#include <cstddef> // std::size_t
#include <tuple>

namespace ds {

template <typename, typename...>
class temp_node;

template <typename T>
temp_node<T> n(T);

template <typename T, typename... Children>
class temp_node {

    friend temp_node<T> n<T>(T);
    template <typename, typename...>
    friend class temp_node;

    public:
    using children_t = const std::tuple<Children...>;
    using value_t    = T;

    public:
    value_t value;
    std::size_t subtree_size  = 1; // Number of nodes of the tree considering this one as root.
    std::size_t subtree_arity = 0; // Arity of the tree having this node as root.
    children_t children;

    private:
    // Constructors are private because the instances must be constructed using function n().
    temp_node(const T& value, Children&&... children) :
            value(value),
            children(std::forward<Children>(children)...) {
        std::size_t size  = 1;
        std::size_t arity = 0;
        auto call         = [&](auto& node) {
            if constexpr (
                std::is_same_v<
                    typename std::remove_reference_t<decltype(node)>::value_t,
                    std::nullptr_t>) {
                return;
            }
            size += node.get_subtree_size();
            arity = std::max(arity, node.get_subtree_arity());
        };
        (call(children), ...);
        this->subtree_arity = arity;
        this->subtree_size  = size;
    }

    public:
    ~temp_node() = default;

    temp_node(const temp_node& other) :
            value(other.value),
            subtree_size(other.subtree_size),
            subtree_arity(other.subtree_arity),
            children(other.children) {
    }

    constexpr bool has_children() const {
        return children_number() > 0;
    }

    constexpr std::size_t children_number() const {
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
    constexpr temp_node<T, Nodes...> operator()(Nodes&&... nodes) const {
        return std::move(temp_node<T, Nodes...>(this->value, std::forward<Nodes>(nodes)...));
    }
};

template <std::size_t index, typename T, typename... Children>
std::tuple_element_t<index, std::tuple<Children...>>
get_child(const temp_node<T, Children...>& node) {
    return std::get<index>(node.children);
}

// Functions to generate temporary_node objects.
template <typename T>
temp_node<T> n(T value) {
    return std::move(temp_node<T>(value));
}

template <typename... Args>
temp_node<std::tuple<Args...>> n(Args... args) {
    return std::move(n(std::make_tuple(args...)));
}

temp_node<std::nullptr_t> n() {
    return std::move(n(nullptr));
}

} // namespace ds
