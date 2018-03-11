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
    using alternative_t = std::variant<std::monostate, temporary_node<T>>;
    using children_t    = std::initializer_list<alternative_t>;

private:
    T value;
    size_t tree_size;
    children_t children;

private:
    temporary_node(T value) :
            value(value),
            tree_size(1),
            children() {
    }

public:
    ~temporary_node() = default;

    temporary_node(temporary_node&& other) :
            value(std::move(other.value)),
            tree_size(other.tree_size),
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
        return count_children(children);
    }

    T get_value() const {
        return value;
    }

    size_t get_size() const {
        return tree_size;
    }

    temporary_node&& operator()(children_t nodes) {
        children = nodes;
        for (auto&& child : children) {
            if (!std::holds_alternative<temporary_node>(child)) {
                continue;
            }
            tree_size += std::get<temporary_node>(child).tree_size;
        }
        return std::move(*this);
    }

    static unsigned count_children(children_t list) {
        return std::count_if(
            list.begin(),
            list.end(),
            [](const alternative_t& child) {
                return std::holds_alternative<temporary_node<T>>(child);
            });
    }
};

// Functions to generate temporary_node objects.
template <typename T>
temporary_node<T> n(T value) {
    return temporary_node<T>(value);
}

std::monostate n() {
    return std::monostate{};
}

} // namespace ds
