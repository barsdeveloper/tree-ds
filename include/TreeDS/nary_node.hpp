#pragma once
#include <TreeDS/temp_node.hpp>
#include <TreeDS/utility.hpp>
#include <utility> // std::move(), std::forward()

namespace ds {

template <typename T>
class nary_node {

    template <typename, typename, bool>
    friend class tree_iterator;

    template <typename, typename, typename>
    friend class tree;

    protected:
    T _value;
    nary_node<T>* _parent;
    nary_node<T>* _next_sibling;
    nary_node<T>* _first_child;

    public:
    // Forward constructor: the arguments are forwarded directly to the constructor of the type T hold into this node.
    template <typename... Args, CHECK_CONSTRUCTIBLE(T, Args...)>
    explicit nary_node(Args&&... args) :
            _value(std::forward<Args>(args)...),
            _parent(nullptr),
            _next_sibling(nullptr),
            _first_child(nullptr) {
    }
};

} // namespace ds
