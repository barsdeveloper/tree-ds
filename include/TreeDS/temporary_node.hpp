#pragma once

#include <initializer_list>
#include <optional>
#include <variant>

namespace ds {

template <typename>
class temporary_node;

template <typename T>
temporary_node<T> n(T);

template <typename T>
class temporary_node {

    friend temporary_node n<T>(T);

public:
    using alternative_t = std::variant<temporary_node<T>, std::monostate>;
    using children_t    = std::initializer_list<alternative_t>;

private:
    T value;
    size_t tree_size;
    size_t subtree_arity;
    size_t max_subtree_arity;
    children_t children;

private:
    template <
        typename... Args,
        typename = std::enable_if_t<std::is_constructible_v<T, Args...>>>
    temporary_node(Args... args) :
            value(args...),
            tree_size(1),
            subtree_arity(0),
            children() {
    }

public:
    ~temporary_node() = default;

    temporary_node(temporary_node&& other) :
            value(std::move(other.value)),
            tree_size(other.tree_size),
            subtree_arity(other.subtree_arity),
            children(other.children) {
        other.tree_size = 0;
    }

    auto begin() const {
        return children.begin();
    }

    auto end() const {
        return children.end();
    }

    bool has_children() const {
        return children_number() > 0;
    }

    unsigned children_number() const {
        return std::count_if(
            children.begin(),
            children.end(),
            std::holds_alternative<temporary_node, temporary_node, std::monostate>);
        //                         ^ desired type, ^... types of the std::variant used
    }

    const temporary_node* get_child(unsigned index) const {
        if (index >= children.size()) {
            return nullptr;
        }
        auto it = children.begin();
        std::advance(it, index);
        return std::get_if<temporary_node>(it);
    }

    T get_value() const {
        return value;
    }

    size_t get_size() const {
        return tree_size;
    }

    constexpr temporary_node&& operator()(children_t nodes) {
        children = nodes;
        for (auto&& child : children) {
            if (std::holds_alternative<temporary_node>(child)) {
                tree_size += std::get<temporary_node>(child).tree_size;
            }
        }
        return std::move(*this);
    }
};

// Functions to generate temporary_node objects.
template <typename T>
temporary_node<T> n(T value) {
    return temporary_node<T>(value);
}

template <typename... Args>
temporary_node<std::tuple<Args...>> n(Args... args) {
    return n(std::make_tuple(args...));
}

std::monostate n() {
    return std::monostate{};
}

} // namespace ds
