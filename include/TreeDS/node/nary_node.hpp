#pragma once

#include <utility> // std::move(), std::forward()

#include <TreeDS/node/node.hpp>
#include <TreeDS/node/struct_node.hpp>
#include <TreeDS/utility.hpp>

namespace ds {

template <typename T>
class nary_node : node<T, nary_node> {

    template <typename, typename, bool>
    friend class tree_iterator;

    protected:
    nary_node* next_sibling = nullptr;
    nary_node* first_child  = nullptr;

    public:
    using node<T, nary_node>::node;

    public:
    /*   ---   Permissive Copy Constructor   ---   */
    /**
     * 
     */
    template <
        typename ConvertibleT = T,
        typename AllocateFn,
        CHECK_CONVERTIBLE(ConvertibleT, T)>
    explicit nary_node(const nary_node<ConvertibleT>& other, AllocateFn allocate) :
            node<T, nary_node<T>>(static_cast<T>(other.value)),
            next_sibling(
                other.next_sibling
                    ? attach(allocate(*other.next_sibling, allocate).release())
                    : nullptr),
            first_child(
                other.first_child
                    ? attach(allocate(*other.first_child, allocate).release())
                    : nullptr) {
    }
};

} // namespace ds
