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

    protected:
    Node* attach(Node* node) {
        assert(node != nullptr);
        node->parent = static_cast<Node*>(this);
        return node;
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

    bool is_first_child() const {
        return parent
            ? this == parent->get_first_child()
            : false;
    }

    bool is_last_child() const {
        return parent
            ? this == parent->get_last_child()
            : false;
    }

    bool is_unique_child() const {
        return parent
            ? this == parent->get_first_child() && this == parent->get_last_child()
            : false;
    }
};

} // namespace ds
