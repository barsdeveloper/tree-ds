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

    /*   ---   ATTRIBUTES   ---   */
    public:
    static constexpr matcher_info_t info {Captured::info};

    /*   ---   CONSTRUCTORS   ---   */
    public:
    using matcher<capture_node, Name, Captured, NextSibling>::matcher;

    /*   ---   METHODS   ---   */
    public:
    template <
        typename NodeAllocator,
        typename Iterator,
        typename AckowledgeFunction = detail::empty_t,
        typename RematchFunction    = detail::empty_t>
    bool search_node_this(
        NodeAllocator&& allocator,
        Iterator&& it,
        AckowledgeFunction&& ackowledge = detail::empty_t(),
        RematchFunction&& rematch       = detail::empty_t()) {
        if (this->get_first_child().search_node(allocator, it, ackowledge, rematch)) {
            this->target_node = this->get_first_child().get_node(allocator);
            return true;
        }
        return false;
    }

    template <typename NodeAllocator, typename Iterator>
    bool search_node_impl(NodeAllocator& allocator, Iterator& it) {
        return this->get_first_child().search_node(allocator, it);
    }

    template <typename NodeAllocator>
    unique_ptr_alloc<NodeAllocator> result_impl(NodeAllocator& allocator) {
        // Node retrieval is delegated to the captured pattern
        return this->get_first_child().result(allocator);
    }

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
