#pragma once
#include <TreeDS/utility.hpp>
#include <TreeDS/node.hpp>
#include <utility> // std::move(), std::forward()

#include "struct_node.hpp"

namespace ds {

template <typename T>
class nary_node : node<T, nary_node> {

    template <typename, typename, bool>
    friend class tree_iterator;

    template <typename, typename, typename>
    friend class tree;

    protected:
    T _value;
    nary_node* _next_sibling = nullptr;
    nary_node* _first_child = nullptr;

    public:
    // Forward constructor: the arguments are forwarded directly to the constructor of the type T hold into this node.
    template <typename... Args, CHECK_CONSTRUCTIBLE(T, Args...)>
    explicit nary_node(Args&&... args) :
            _value(std::forward<Args>(args)...){
    }
};

} // namespace ds
