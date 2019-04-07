#pragma once

#include <tuple>
#include <utility> // std::forward()

#include <TreeDS/utility.hpp>

namespace md {

template <typename T, typename Node>
class node {

    public:
    using value_type = T;

    protected:
    T value;
    Node* parent = nullptr;

    protected:
    // Forward constructor: the arguments are forwarded directly to the constructor of the type T.
    template <typename... Args, CHECK_CONSTRUCTIBLE(T, Args...)>
    explicit node(Node* parent, Args&&... args) :
            value(args...),
            parent(parent) {
    }

    // Forward constructor: the arguments are forwarded directly to the constructor of the type T (packed as tuple).
    template <typename... Args, CHECK_CONSTRUCTIBLE(T, Args...)>
    explicit node(Node* parent, const std::tuple<Args...>& args_tuple) :
            value(std::make_from_tuple<T>(std::move(args_tuple))),
            parent(parent) {
    }

    public:
    // Forward constructor: the arguments are forwarded directly to the constructor of the type T.
    template <typename... Args, CHECK_CONSTRUCTIBLE(T, Args...)>
    explicit node(Args&&... args) :
            node(nullptr, std::forward<Args>(args)...) {
    }

    // Forward constructor: the arguments are forwarded directly to the constructor of the type T (packed as tuple).
    template <typename... Args, CHECK_CONSTRUCTIBLE(T, Args...)>
    explicit node(const std::tuple<Args...>& args_tuple) :
            node(nullptr, args_tuple) {
    }

    public:
    /*   ---   Getters   ---   */
    const T& get_value() const {
        return value;
    }

    T& get_value() {
        return value;
    }

    const Node* get_parent() const {
        return parent;
    }

    Node* get_parent() {
        return parent;
    }

    Node* get_parent_limit(const Node& root) {
        return this->is_root_limit(root) ? nullptr : this->get_parent();
    }

    const Node* get_parent_limit(const Node& root) const {
        return this->is_root_limit(root) ? nullptr : this->get_parent();
    }

    bool is_root_limit(const Node& root) const {
        return this == &root;
    }

    bool is_root() const {
        return parent == nullptr;
    }

    bool has_children() const {
        return static_cast<const Node*>(this)->get_first_child() != nullptr;
    }

    bool is_unique_child() const {
        return static_cast<const Node*>(this)->is_first_child()
            && static_cast<const Node*>(this)->is_last_child();
    }
};

} // namespace md
