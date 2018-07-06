#pragma once

#include <TreeDS/utility.hpp>
#include <utility> // std::forward()

namespace ds {

template <typename T, typename Node>
class node {

    protected:
    T _value;
    Node* _parent = nullptr;

    public:
    // Forward constructor: the arguments are forwarded directly to the constructor of the type T hold into this node.
    template <typename... Args, CHECK_CONSTRUCTIBLE(T, Args...)>
    explicit node(Args&&... args) :
            _value(std::forward<Args>(args)...) {
    }

    public:
    /*   ---   Getters   ---   */
    const T value() const {
        return _value;
    }

    T value() {
        return _value;
    }

    const Node* parent() const {
        return _parent;
    }
};

} // namespace ds
