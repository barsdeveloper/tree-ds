#pragma once

#ifndef NDEBUG
#include <TreeDS/node/navigator/node_navigator.hpp>
#include <TreeDS/policy/breadth_first.hpp>
#endif

#include <tuple>
#include <utility> // std::forward()

#include <TreeDS/utility.hpp>

namespace md {

template <typename T, typename Node>
class node {

    /*   ---   TYPES   ---   */
    public:
    using value_type = T;

    /*   ---   ATTRIBUTES   ---   */
    protected:
    T value;
    Node* parent = nullptr;

    /*   ---   CONSTRUCTORS   ---   */
    protected:
    // Forward constructor: the arguments are forwarded directly to the constructor of the type T
    template <typename... Args, typename = std::enable_if_t<std::is_constructible_v<T, Args...>>>
    explicit node(Node* parent, Args&&... args) :
            value(args...),
            parent(parent) {
    }

    // Forward constructor: the arguments are forwarded directly to the constructor of the type T (packed as tuple)
    template <typename... Args, typename = std::enable_if_t<std::is_constructible_v<T, Args...>>>
    explicit node(Node* parent, const std::tuple<Args...>& args_tuple) :
            value(std::make_from_tuple<T>(std::move(args_tuple))),
            parent(parent) {
    }

    public:
    // Forward constructor: the arguments are forwarded directly to the constructor of the type T
    template <typename... Args, typename = std::enable_if_t<std::is_constructible_v<T, Args...>>>
    explicit node(Args&&... args) :
            node(nullptr, std::forward<Args>(args)...) {
    }

    // Forward constructor: the arguments are forwarded directly to the constructor of the type T (packed as tuple)
    template <typename... Args, typename = std::enable_if_t<std::is_constructible_v<T, Args...>>>
    explicit node(const std::tuple<Args...>& args_tuple) :
            node(nullptr, args_tuple) {
    }

    public:
    /*   ---   GETTERS   ---   */
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

    Node* get_prev_sibling() const {
        return static_cast<const Node*>(this)->get_prev_sibling();
    }

    Node* get_next_sibling() const {
        return static_cast<const Node*>(this)->get_next_sibling();
    }

    Node* get_first_child() const {
        return static_cast<const Node*>(this)->get_first_child();
    }

    Node* get_last_child() const {
        return static_cast<const Node*>(this)->get_first_child();
    }

#ifndef NDEBUG
    const Node* get_root() const {
        const Node* root    = static_cast<const Node*>(this);
        const Node* current = this->get_parent();
        while (current) {
            root    = current;
            current = current->get_parent();
        }
        return root;
    }

    std::size_t get_breadth_first_node_index() const {
        std::size_t index = 0;
        auto iterator     = policy::breadth_first().get_instance(
            this->get_root(),
            node_navigator<const Node*>(this->get_root()),
            std::allocator<Node>());
        while (iterator.get_current_node() && iterator.get_current_node() != this) {
            iterator.increment();
            ++index;
        }
        return index;
    }
#endif

    /*   ---   METHODS   ---   */
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
