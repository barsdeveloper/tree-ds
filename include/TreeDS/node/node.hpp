#pragma once

#include <TreeDS/utility.hpp>
#include <utility> // std::forward()

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

    Node* get_parent() {
        return parent;
    }

    bool is_first_child() const {
        return parent
            ? this == parent->first_child()
            : false;
    }

    bool is_last_child() const {
        return parent
            ? this == parent->last_child()
            : false;
    }

    bool is_unique_child() const {
        return parent
            ? this == parent->first_child() && this == parent->last_child()
            : false;
    }
};

} // namespace ds
