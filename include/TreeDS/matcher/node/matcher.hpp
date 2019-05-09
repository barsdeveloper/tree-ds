#pragma once

#include <functional> // std::reference_wrapper
#include <optional>

#include <TreeDS/matcher/value/true_matcher.hpp>
#include <TreeDS/node/struct_node.hpp>
#include <TreeDS/tree.hpp>

namespace md {

namespace detail {
    struct capture_tag_t {};
}; // namespace detail

struct matcher_info_t {
    bool matches_null;
    bool reluctant;
    constexpr matcher_info_t(bool matches_null, bool reluctant) :
            matches_null(matches_null),
            reluctant(reluctant) {
    }
};

template <
    template <typename, typename...> class Derived,
    typename ValueMatcher,
    typename... Children>
class matcher : public struct_node<ValueMatcher, Children...> {

    /*   ---   TYPES   ---   */
    protected:
    using derived_t  = Derived<ValueMatcher, Children...>;
    using captures_t = decltype(std::tuple_cat(
        std::declval<
            std::conditional_t<
                std::is_same_v<ValueMatcher, detail::capture_tag_t>,
                std::tuple<matcher&>,
                std::tuple<>>>(),
        std::declval<typename Children::captures_t>()...));

    /*   ---   ATTRIBUTES   ---   */
    protected:
    void* target_node   = nullptr;
    captures_t captures = std::tuple_cat(
        [&]() {
            if constexpr (std::is_same_v<ValueMatcher, detail::capture_tag_t>) {
                return std::tie(*this);
            } else {
                return std::make_tuple();
            }
        }(),
        std::apply(
            [](auto&&... children) {
                return std::tuple_cat(children.captures...);
            },
            this->children));

    /*   ---   CONSTRUCTORS   ---   */
    public:
    using struct_node<ValueMatcher, Children...>::struct_node;

    /*   ---   METHODS   ---   */
    protected:
    template <typename Node>
    static auto get_children_supplier(Node& target) {
        return [node = target.get_first_child()]() mutable {
            Node* result = node;
            node         = node->get_next_sibling();
            return result;
        };
    }

    template <typename Value>
    bool match_value(const Value& value) {
        if constexpr (std::is_same_v<ValueMatcher, true_matcher>) {
            return true;
        } else {
            return this->value == value;
        }
    }

    template <typename NodeSupplier>
    bool match_children(NodeSupplier supplier) {
        if constexpr (matcher::children_count() == 0) {
            return true;
        } else {
            auto call = [&](auto&& node) {
                return node.match_node(supplier());
            };
            return std::apply(
                [&](auto&&... nodes) {
                    return (... && call(nodes));
                },
                this->children);
        }
    }

    public:
    template <typename Node>
    bool match_node(Node* node) {
        if (derived_t::info.matches_null && node == nullptr) {
            return true;
        }
        if (static_cast<derived_t*>(this)->match_node_impl(*node)) {
            this->target_node = node;
            return true;
        }
        return false;
    }
    template <typename NodeAllocator>
    unique_node_ptr<NodeAllocator> get_matched_node(NodeAllocator&& allocator) {
        return static_cast<derived_t*>(this)->get_matched_node_impl(allocator);
    }

    std::size_t capture_size() const {
        return std::tuple_size_v<captures_t>;
    }

    template <typename... Nodes>
    constexpr Derived<ValueMatcher, Nodes...> operator()(Nodes&&... nodes) const {
        return {this->value, std::move(nodes)...};
    }
};

} // namespace md
