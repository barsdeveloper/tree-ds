#pragma once

#include <functional> // std::reference_wrapper
#include <optional>

#include <TreeDS/matcher/node/matcher.hpp>
#include <TreeDS/matcher/pattern.hpp>
#include <TreeDS/matcher/value/true_matcher.hpp>

namespace md {

template <typename Name, typename Captured, typename NextSibling = detail::empty_t>
class capture_node : public matcher<
                         capture_node<Name, Captured, NextSibling>,
                         Name,
                         Captured,
                         NextSibling> {

    /*   ---   FRIENDS   ---   */
    template <typename, typename, typename, typename>
    friend class matcher;
    friend class pattern<capture_node>;

    /*   ---   ATTRIBUTES   ---   */
    public:
    static constexpr matcher_info_t info {Captured::info};

    /*   ---   CONSTRUCTORS   ---   */
    public:
    using matcher<capture_node, Name, Captured, NextSibling>::matcher;

    /*   ---   METHODS   ---   */
    protected:
    template <typename NodeAllocator>
    bool search_node_impl(allocator_value_type<NodeAllocator>& node, NodeAllocator& allocator) {
        return std::get<0>(this->get_children()).search_node(&node, allocator);
    }

    template <typename NodeAllocator>
    unique_node_ptr<NodeAllocator> result_impl(NodeAllocator& allocator) {
        // Node retrieval is delegated to the captured pattern
        return std::get<0>(this->get_children()).result(allocator);
    }

    public:
    template <typename... Nodes>
    constexpr capture_node<Name, Nodes...> replace_children(Nodes&... nodes) const {
        static_assert(
            sizeof...(Nodes) == 0,
            "Just a single node can be captured and must be provided as argument: cpt(one(1)), not as children: cpt(...)(one(1))");
        return {this->value, nodes...};
    }

    template <typename Child>
    constexpr capture_node<Name, std::remove_reference_t<Child>, NextSibling>
    with_first_child(Child&& child) const {
        return {this->value, child, this->next_sibling};
    }

    template <typename Sibling>
    constexpr capture_node<Name, Captured, std::remove_reference_t<Sibling>>
    with_next_sibling(Sibling&& sibling) const {
        return capture_node<Name, Captured, std::remove_reference_t<Sibling>>(
            this->value,
            this->first_child,
            sibling);
    }
};

template <typename Captured>
capture_node<capture_name<>, Captured, detail::empty_t> cpt(Captured&& capture) {
    return capture_node<capture_name<>, std::decay_t<Captured>, detail::empty_t>(
        capture_name<>(),
        std::move(capture),
        detail::empty_t());
}

template <typename Name, typename Captured>
capture_node<std::decay_t<Name>, std::decay_t<Captured>, detail::empty_t> cpt(Name&&, Captured&& capture) {
    static_assert(is_same_template<std::decay_t<Name>, capture_name<>>, "Argument name must be a proper capture_name type.");
    return capture_node<std::decay_t<Name>, std::decay_t<Captured>, detail::empty_t>(
        std::decay_t<Name>(),
        std::move(capture),
        detail::empty_t());
}

} // namespace md
