#pragma once

#include <initializer_list>
#include <optional>
#include <utility>
#include <variant>

namespace ds {

template <typename>
class temp_node;

template <typename T>
temp_node<T> n(T);

template <typename T>
class temp_node {

    friend temp_node n<T>(T);

    public:
    using alternative_t = std::variant<temp_node<T>, std::monostate>;
    using children_t    = std::initializer_list<alternative_t>;

    private:
    T value;
    size_t subtree_size;  // Number of nodes of the tree considering this one as root.
    size_t subtree_arity; // Arity of the tree having this node as root.
    children_t children;

    private:
    // This is private because the object must be constructed using function n().
    template <
        typename... Args,
        typename = std::enable_if_t<std::is_constructible_v<T, Args...>>>
    temp_node(Args... args) :
            value(args...),
            subtree_size(1),
            subtree_arity(0),
            children() {
    }

    public:
    ~temp_node() = default;

    temp_node(temp_node&& other) :
            value(std::move(other.value)),
            subtree_size(other.subtree_size),
            subtree_arity(other.subtree_arity),
            children(other.children) {
        other.subtree_size = 0;
    }

    constexpr auto begin() const {
        return this->children.begin();
    }

    constexpr auto end() const {
        return this->children.end();
    }

    constexpr bool has_children() const {
        return children_number() > 0;
    }

    constexpr unsigned children_number() const {
        int result = 0;
        for (const auto& child : this->children) {
            if (std::holds_alternative<temp_node, temp_node, std::monostate>(child)) {
                ++result;
            }
        }
        return result;
    }

    const temp_node* get_child(unsigned index) const {
        if (index >= children.size()) {
            return nullptr;
        }
        auto it = children.begin();
        std::advance(it, index);
        return std::get_if<temp_node>(it);
    }

    constexpr T get_value() const {
        return this->value;
    }

    constexpr size_t get_subtree_size() const {
        return this->subtree_size;
    }

    constexpr size_t get_subtree_arity() const {
        return this->subtree_arity;
    }

    constexpr temp_node&& operator()(children_t nodes) {
        this->children = nodes;
        for (const auto& child : this->children) {
            if (std::holds_alternative<temp_node>(child)) {
                this->subtree_size += std::get<temp_node>(child).subtree_size;
            }
        }
        return std::move(*this);
    }
};

// Functions to generate temporary_node objects.
template <typename T>
temp_node<T> n(T value) {
    return temp_node<T>(value);
}

template <typename... Args>
temp_node<std::tuple<Args...>> n(Args... args) {
    return n(std::make_tuple(args...));
}

std::monostate n() {
    return std::monostate{};
}

} // namespace ds
