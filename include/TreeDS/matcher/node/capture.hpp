#pragma once

#include <functional> // std::reference_wrapper
#include <optional>

#include <TreeDS/matcher/node/matcher.hpp>
#include <TreeDS/matcher/pattern.hpp>
#include <TreeDS/matcher/value/true_matcher.hpp>

namespace md {

template <typename Name, typename Captured>
class capture_node : public matcher<capture_node, Name, Captured> {

    /*   ---   FRIENDS   ---   */
    template <template <typename, typename...> class, typename, typename...>
    friend class matcher;
    friend class pattern<capture_node>;

    /*   ---   TYPES   ---   */
    public:
    using super = matcher<capture_node, Name, Captured>;
    using typename super::captures_t;

    /*   ---   ATTRIBUTES   ---   */
    public:
    static constexpr matcher_info_t info {Captured::info};

    /*   ---   CONSTRUCTORS   ---   */
    public:
    capture_node(Captured&& captured_pattern) :
            super(
                Name(),
                std::move(captured_pattern)) {
    }

    /*   ---   METHODS   ---   */
    protected:
    template <typename Node>
    bool match_node_impl(Node& node) {
        return std::get<0>(this->children).match_node(&node);
    }

    template <typename NodeAllocator>
    unique_node_ptr<NodeAllocator> get_matched_node_impl(NodeAllocator& allocator) {
        // Node retrieval is delegated to the captured pattern
        return std::get<0>(this->children).get_matched_node(allocator);
    }

    template <typename NodeAllocator>
    void attach_matched_impl(allocator_value_type<NodeAllocator>& target, NodeAllocator& allocator) {
        return std::get<0>(this->children).attach_matched(target, allocator);
    }

    public:
    template <typename... Nodes>
    constexpr capture_node operator()(Nodes&&...) const {
        static_assert(
            sizeof...(Nodes) == 0,
            "Just a single node can be captured and must be provided as argument: cpt(one(1)), not as children: cpt(...)(one(1))");
        return {};
    }
};

template <typename Captured>
capture_node<capture_name<>, Captured> cpt(Captured&& capture) {
    return {std::move(capture)};
}

template <typename Name, typename Captured>
capture_node<std::decay_t<Name>, std::decay_t<Captured>> cpt(Name&&, Captured&& capture) {
    static_assert(is_same_template<std::decay_t<Name>, capture_name<>>, "Argument name must be a proper capture_name type.");
    return {std::move(capture)};
}

} // namespace md
