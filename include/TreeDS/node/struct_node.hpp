#pragma once

#include <algorithm>
#include <cstddef> // std::size_t
#include <tuple>   // std::tuple
#include <type_traits>

#include <TreeDS/utility.hpp>

namespace md {

template <typename Derived, typename T, typename FirstChild, typename NextSibling>
class struct_node_base {

    /*   ---   VALIDATION   ---   */
    static_assert(
        !(is_empty<T> && !is_empty<FirstChild>),
        "An emtpy node n() cannot have children: n()(something)");
    static_assert(!is_empty_node<T>, "A node cannot carry an empty node: n(n()).");

    /*   ---   FRIENDS   ---   */
    template <typename, typename, typename, typename>
    friend class struct_node_base;

    /*   ---   TYPES   ---   */
    public:
    using value_t        = T;
    using first_child_t  = std::decay_t<FirstChild>;
    using next_sibling_t = std::decay_t<NextSibling>;

    /*   ---   ATTRIBUTES   ---   */
    protected:
    value_t value;            // Value hold by this node
    FirstChild first_child;   // First child
    NextSibling next_sibling; // Next sibling
    std::size_t index = 0;    // The index of this node as a child

    /*   ---   CONSTRUCTORS   ---   */
    public:
    constexpr struct_node_base(const T& value, const FirstChild& first_child, const NextSibling& next_sibling) :
            value(value),
            first_child(first_child),
            next_sibling(next_sibling) {
        if constexpr (struct_node_base::has_first_child()) {
            this->first_child.assign_indexes();
        }
    }

    ~struct_node_base() = default;

    constexpr struct_node_base(const struct_node_base& other) = default;

    /*   ---   METHODS   ---   */
    private:
    template <std::size_t Index>
    auto get_successors(const_index<Index>) const {
        if constexpr (Index == 0) {
            return std::tie(*this->cast());
        } else {
            return std::tuple_cat(
                std::tie(*this->cast()),
                this->next_sibling.get_successors(const_index<Index - 1>()));
        }
    }

    template <std::size_t Index>
    auto get_successors(const_index<Index>) {
        if constexpr (Index == 0) {
            return std::tie(*this->cast());
        } else {
            return std::tuple_cat(
                std::tie(*this->cast()),
                this->next_sibling.get_successors(const_index<Index - 1>()));
        }
    }

    template <typename Func, typename Initial>
    static constexpr auto foldl_siblings_types(Func&& f, Initial&& initial) {
        if constexpr (struct_node_base::has_next_sibling()) {
            return NextSibling::foldl_siblings_types(f, f(initial, type_value<NextSibling>()));
        } else {
            return initial;
        }
    }

    template <typename Func, typename Initial>
    constexpr auto foldl_siblings(Func&& f, Initial&& initial) {
        if constexpr (struct_node_base::has_next_sibling()) {
            return this->next_sibling.foldl_siblings(f, f(initial, this->next_sibling));
        } else {
            return initial;
        }
    }

    template <typename Func, typename Initial>
    static constexpr auto foldr_siblings_types(Func&& f, Initial&& initial) {
        if constexpr (struct_node_base::has_next_sibling()) {
            return f(type_value<NextSibling>(), NextSibling::foldr_siblings_types(f, initial));
        } else {
            return initial;
        }
    }

    template <typename Func, typename Initial>
    constexpr auto foldr_siblings(Func&& f, Initial&& initial) {
        if constexpr (struct_node_base::has_next_sibling()) {
            return f(this->next_sibling, this->get_next_sibling().foldr_siblings(f, initial));
        } else {
            return initial;
        }
    }

    void assign_indexes(std::size_t index = 0) {
        this->index = index;
        if constexpr (struct_node_base::has_next_sibling()) {
            this->next_sibling.assign_indexes(index + 1);
        }
    }

    protected:
    const Derived* cast() const {
        return static_cast<const Derived*>(this);
    }
    Derived* cast() {
        return static_cast<Derived*>(this);
    }

    public:
    const FirstChild& get_first_child() const {
        return this->first_child;
    }

    FirstChild& get_first_child() {
        return this->first_child;
    }

    NextSibling& get_next_sibling() {
        return this->next_sibling;
    }

    const NextSibling& get_next_sibling() const {
        return this->next_sibling;
    }

    template <typename Func, typename Initial>
    static constexpr auto foldl_children_types(Func&& f, Initial&& initial) {
        if constexpr (struct_node_base::has_first_child()) {
            return FirstChild::foldl_siblings_types(f, f(initial, type_value<FirstChild>()));
        } else {
            return initial;
        }
    }

    template <typename Func, typename Initial>
    constexpr auto foldl_children(Func&& f, Initial&& initial) {
        if constexpr (struct_node_base::has_first_child()) {
            return this->first_child.foldl_siblings(f, f(initial, this->first_child));
        } else {
            return initial;
        }
    }

    template <typename Func, typename Initial>
    static constexpr auto foldr_children_types(Func&& f, Initial&& initial) {
        if constexpr (struct_node_base::has_first_child()) {
            return f(type_value<FirstChild>(), FirstChild::foldr_siblings_type(f, initial));
        } else {
            return initial;
        }
    }

    template <typename Func, typename Initial>
    constexpr auto foldr_children(Func&& f, Initial&& initial) {
        if constexpr (struct_node_base::has_first_child) {
            return f(this->first_child, this->first_child.foldr_siblings(f, initial));
        } else {
            return initial;
        }
    }

    static constexpr bool is_valid_node() {
        return !is_empty<T>;
    }
    static constexpr bool has_first_child() {
        return !is_empty<FirstChild>;
    }
    static constexpr bool has_next_sibling() {
        return !is_empty<NextSibling>;
    }

    constexpr T get_value() const {
        return this->value;
    }

    static constexpr std::size_t children() {
        return struct_node_base::foldl_children_types(
            [](auto&& accumulated, auto&& element) {
                using element_type = typename std::remove_reference_t<decltype(element)>::type;
                if constexpr (element_type::is_valid_node()) {
                    // if element is not just n(), but n(something)
                    return accumulated + 1u;
                } else {
                    return accumulated;
                }
            },
            0u);
    }

    static constexpr std::size_t children_all() {
        return struct_node_base::foldl_children_types(
            [](auto&& accumulated, auto&&) {
                return accumulated + 1u;
            },
            0u);
    }

    static constexpr std::size_t subtree_size() {
        return struct_node_base::foldl_children_types(
            [](auto&& accumulated, auto&& element) {
                using element_type = typename std::remove_reference_t<decltype(element)>::type;
                return accumulated + element_type::subtree_size();
            },
            struct_node_base::is_valid_node() ? 1u : 0u);
    }

    static constexpr std::size_t subtree_arity() {
        return struct_node_base::foldl_children_types(
            [](auto&& accumulated, auto&& element) {
                using element_type = typename std::remove_reference_t<decltype(element)>::type;
                return std::max({accumulated, struct_node_base::children(), element_type::subtree_arity()});
            },
            static_cast<std::size_t>(0u));
    }
    constexpr std::size_t get_index() const {
        return this->index;
    }

    template <typename... Nodes>
    constexpr auto operator()(Nodes&&... nodes) const {
        if constexpr (sizeof...(Nodes) > 0) {
            return this->cast()->with_first_child(
                foldr(
                    [](auto&& element, auto&& accumulated) {
                        return element.with_next_sibling(accumulated);
                    },
                    detail::empty_t(),
                    nodes...));
        } else {
            return n(this->value);
        }
    }

    static constexpr std::size_t following_siblings() {
        if constexpr (!is_empty<NextSibling>) {
            return 1u + NextSibling::following_siblings();
        } else {
            return 0u;
        }
    }

    constexpr auto get_children() const {
        if constexpr (struct_node_base::has_first_child()) {
            return this->first_child.get_successors(const_index<struct_node_base::children_all() - 1>());
        } else {
            return std::make_tuple();
        }
    }

    constexpr auto get_children() {
        if constexpr (struct_node_base::has_first_child()) {
            return this->first_child.get_successors(const_index<struct_node_base::children_all() - 1>());
        } else {
            return std::make_tuple();
        }
    }

    template <std::size_t Index>
    constexpr auto& get_child(const_index<Index>) const {
        //static_assert(Index < struct_node::children(), "The index of the child is out of range.");
        return std::get<Index>(this->get_children());
    }
};

template <typename T, typename FirstChild = detail::empty_t, typename NextSibling = detail::empty_t>
class struct_node : public struct_node_base<struct_node<T, FirstChild, NextSibling>, T, FirstChild, NextSibling> {
    public:
    using struct_node_base<struct_node, T, FirstChild, NextSibling>::struct_node_base;

    template <typename Child>
    constexpr struct_node<T, std::remove_reference_t<Child>, NextSibling>
    with_first_child(Child&& child) const {
        return {this->value, child, this->next_sibling};
    }

    template <typename Sibling>
    constexpr struct_node<T, FirstChild, std::remove_reference_t<Sibling>>
    with_next_sibling(Sibling&& sibling) const {
        return {this->value, this->first_child, sibling};
    }
};

// Functions to generate struct_node objects
template <typename T>
constexpr struct_node<T> n(T value) {
    return struct_node<T>(value, detail::empty_t(), detail::empty_t());
}

template <typename... Args>
constexpr struct_node<std::tuple<Args...>> n(Args... args) {
    return struct_node<std::tuple<Args...>>(std::make_tuple(args...), detail::empty_t(), detail::empty_t());
}

constexpr struct_node<detail::empty_t> n() {
    return struct_node<detail::empty_t>(detail::empty_t(), detail::empty_t(), detail::empty_t());
}

} // namespace md
