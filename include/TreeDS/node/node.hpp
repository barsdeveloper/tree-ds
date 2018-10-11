#pragma once

#include <utility> // std::forward()

#include <TreeDS/utility.hpp>

namespace ds {

template <typename T, typename Node>
class node {

    protected:
    T value;
    Node* parent = nullptr;

    public:
    // Forward constructor: the arguments are forwarded directly to the constructor of the type T hold into this node.
    template <typename... Args, CHECK_CONSTRUCTIBLE(T, Args...)>
    explicit node(Args&&... args) :
            value(args...) {
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

    bool is_root() const {
        return parent == nullptr;
    }

    bool is_unique_child() const {
        return static_cast<const Node*>(this)->is_first_child()
            && static_cast<const Node*>(this)->is_last_child();
    }
};

} // namespace ds
