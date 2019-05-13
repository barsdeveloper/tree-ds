#pragma once

#include <cstddef>     // std::size_t
#include <functional>  // std::function
#include <type_traits> // std::is_convertible_v

#include <TreeDS/matcher/value/true_matcher.hpp>
#include <TreeDS/node/struct_node.hpp>
#include <TreeDS/tree.hpp>

namespace md {

template <
    template <typename, typename...> class,
    typename,
    typename...>
class matcher;

template <typename, typename>
class capture_node;

template <char... name>
struct capture_name {};

struct matcher_info_t {
    bool matches_null;
    bool reluctant;
    constexpr matcher_info_t(bool matches_null, bool reluctant) :
            matches_null(matches_null),
            reluctant(reluctant) {
    }
};

namespace detail {
    template <typename T>
    constexpr bool is_capture_name = is_same_template<T, capture_name<>> && !std::is_same_v<T, capture_name<>>;

    template <typename, typename>
    constexpr std::size_t index_of_capture_name = 1;

    template <char... Name, typename Captured, typename... Types>
    constexpr std::size_t index_of_capture_name<
        matcher<capture_node, capture_name<Name...>, Captured>&,
        std::tuple<Types...>> = 0;

    template <class T, class U, typename... Types>
    constexpr std::size_t index_of_capture_name<T, std::tuple<U, Types...>> = 1 + index_of_capture_name<T, std::tuple<Types...>>;
}; // namespace detail

template <typename T>
T decl() noexcept;

template <
    template <typename, typename...> class Derived,
    typename ValueMatcher,
    typename... Children>
class matcher : public struct_node<ValueMatcher, Children...> {

    /*   ---   TYPES   ---   */
    protected:
    using derived_t           = Derived<ValueMatcher, Children...>;
    using children_captures_t = decltype(std::tuple_cat(decl<typename Children::captures_t>()...));
    using captures_t          = decltype(std::tuple_cat(
        decl<
            std::conditional_t<
                // if this is a capture node
                is_same_template<ValueMatcher, capture_name<>>,
                std::tuple<matcher&>,
                std::tuple<>>>(),
        decl<children_captures_t>()));

    static_assert(
        // It must not happen that:
        !(
            detail::is_capture_name<ValueMatcher>
            // There exists another node...
            && detail::index_of_capture_name<
                   // ...with the same ValueMatcher...
                   matcher,
                   // ...in the captures of the children.
                   children_captures_t>
                // The maximum value is what index_of_capture_name retuns when no such element was found.
                < std::tuple_size_v<children_captures_t>),
        "Named captures must have unique names.");

    /*   ---   ATTRIBUTES   ---   */
    protected:
    void* target_node   = nullptr;
    captures_t captures = std::tuple_cat(
        [this]() {
            if constexpr (is_same_template<ValueMatcher, capture_name<>>) {
                return std::tie(*this);
            } else {
                return std::make_tuple();
            }
        }(),
        std::apply(
            [](auto&... child) {
                return std::tuple_cat(child.captures...);
            },
            this->children));

    /*   ---   CONSTRUCTORS   ---   */
    public:
    using struct_node<ValueMatcher, Children...>::struct_node;

    /*
     * This copy constructor is needed because the implicitly generated one would rewrite the attribute captures (which
     * stores references) using the values from other. Instead we need that attribute to be calculated every time a new
     * object is constructed. This is not a problem since this calculation happens at compile time.
     */
    matcher(const matcher& other) :
            struct_node<ValueMatcher, Children...>(other),
            target_node(other.target_node) {
    }

    /*   ---   METHODS   ---   */
    protected:
    template <typename Node>
    static auto get_children_supplier(Node& target) {
        return [node = target.get_first_child()]() mutable -> Node* {
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
                [&](auto&... nodes) {
                    return (... && call(nodes));
                },
                this->children);
        }
    }

    template <typename NodeTargetSupplier, typename NodeAllocator>
    void attach_matched_children(NodeTargetSupplier target_supplier, NodeAllocator& allocator) {
        // Check that NodeTargetSupplier is some callable that returns a (not null) pointer to node.
        static_assert(
            std::is_convertible_v<
                NodeTargetSupplier,
                std::function<allocator_value_type<NodeAllocator>*()>>,
            "Argument target_supplier must be a callable that returns a (possibly null) pointer to node type.");
        if constexpr (matcher::children_count() > 0) {
            auto* target = target_supplier();
            auto call    = [&](auto&& node) {
                if (node.target_node != nullptr) {
                    node.attach_matched(*target, allocator);
                    target = target_supplier(); // Current target was managed, go to the next one.
                }
            };
            std::apply(
                [&](auto&... nodes) {
                    (..., call(nodes));
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
    unique_node_ptr<NodeAllocator> get_matched_node(NodeAllocator& allocator) {
        return static_cast<derived_t*>(this)->get_matched_node_impl(allocator);
    }

    template <std::size_t index, typename NodeAllocator>
    unique_node_ptr<NodeAllocator> get_captured_node(NodeAllocator& allocator) {
        return std::get<index>(this->captures).get_matched_node(allocator);
    }

    template <typename NodeAllocator>
    void attach_matched(allocator_value_type<NodeAllocator>& target, NodeAllocator& allocator) {
        return static_cast<derived_t*>(this)->attach_matched_impl(target, allocator);
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
