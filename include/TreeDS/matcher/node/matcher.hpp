#pragma once

#include <cstddef>     // std::size_t
#include <type_traits> // std::is_same_v, std::is_convertible_v

#include <TreeDS/matcher/utility.hpp>
#include <TreeDS/matcher/value/true_matcher.hpp>
#include <TreeDS/node/struct_node.hpp>
#include <TreeDS/tree.hpp>

namespace md {

template <typename Derived, typename ValueMatcher, typename... Children>
class matcher : public struct_node<ValueMatcher, Children...> {

    /*   ---   TYPES   ---   */
    protected:
    using children_captures_t = decltype(std::tuple_cat(std::declval<typename Children::captures_t>()...));
    using captures_t          = decltype(std::tuple_cat(
        std::declval<
            std::conditional_t<
                // If this is a capture node
                is_same_template<ValueMatcher, capture_name<>>,
                std::tuple<matcher&>,
                std::tuple<>>>(),
        std::declval<children_captures_t>()));

    /*   ---   VALIDATION   ---   */
    static_assert(
        !(
            // This capture has a (non empty) name
            detail::is_capture_name<ValueMatcher>
            // There exists another capture among children with the same name
            && detail::is_valid_name<ValueMatcher, children_captures_t>),
        "Named captures must have unique names.");

    /*   ---   ATTRIBUTES   ---   */
    protected:
    std::size_t steps   = 1;
    void* referred_node = nullptr;
    captures_t captures = std::tuple_cat(
        [&]() {
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
            referred_node(other.referred_node) {
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

    template <typename MatchFunction, typename RematchFunction = std::nullptr_t>
    bool match_children(MatchFunction& do_match, RematchFunction do_rematch = nullptr) {
        if constexpr (matcher::children_count() > 0) {
            auto check_prefers_null = [&](auto& child) -> bool {
                return child.info.matches_null && child.info.reluctant;
            };
            int current_child;
            for (current_child = 0; current_child < static_cast<int>(this->children_count()); ++current_child) {
                if (apply_at_index(check_prefers_null, this->children, current_child)) {
                    continue;
                }
                if (!apply_at_index(do_match, this->children, current_child)) {
                    if constexpr (std::is_same_v<RematchFunction, std::nullptr_t>) {
                        return false;
                    } else {
                        while (--current_child >= 0) {
                            if (apply_at_index(do_rematch, this->children, current_child)) {
                                break; // We found a child that could rematch and leave the other children new nodes
                            }
                        }
                    }
                }
            }
            return current_child == this->children_count();
        }
        return true;
    }

    public:
    bool empty() const {
        return this->referred_node == nullptr;
    }

    template <typename NodeAllocator>
    allocator_value_type<NodeAllocator>* get_node(NodeAllocator&) const {
        return static_cast<allocator_value_type<NodeAllocator>*>(this->referred_node);
    }

    template <typename NodeAllocator>
    unique_node_ptr<NodeAllocator> clone_node(NodeAllocator& allocator) const {
        return allocate(allocator, this->get_node(allocator)->get_value());
    }

    void reset() {
        this->referred_node = nullptr;
        std::apply(
            [](auto&... child) {
                (..., child.reset());
            },
            this->children);
    }

    template <typename NodeAllocator>
    bool match_node(allocator_value_type<NodeAllocator>* node, NodeAllocator&& allocator) {
        if (node == nullptr) {
            return Derived::info.matches_null;
        }
        if (static_cast<Derived*>(this)->match_node_impl(*node, allocator)) {
            this->referred_node = node;
            return true;
        }
        return false;
    }

    template <typename NodeAllocator>
    // Derived may want to rewrite this
    bool match_node_shallow(allocator_value_type<NodeAllocator>* node, NodeAllocator& allocator) {
        return this->match_node(node, allocator);
    }

    template <typename NodeAllocator>
    unique_node_ptr<NodeAllocator> result(NodeAllocator& allocator) {
        if (this->referred_node == nullptr) {
            return nullptr;
        }
        return static_cast<Derived*>(this)->result_impl(allocator);
    }

    template <std::size_t Index, typename NodeAllocator>
    unique_node_ptr<NodeAllocator> marked_result(capture_index<Index>, NodeAllocator& allocator) {
        static_assert(Index < std::tuple_size_v<captures_t>, "There is no capture with the index requested.");
        return std::get<Index>(this->captures).result(allocator);
    }

    template <char... Name, typename NodeAllocator>
    unique_node_ptr<NodeAllocator> marked_result(capture_name<Name...>, NodeAllocator& allocator) {
        static_assert(
            sizeof...(Name) > 0,
            "Capture's name must be not empty. For example capture_name<'a'> is OK, while capture_name<> is not.");
        static_assert(
            detail::is_valid_name<capture_name<Name...>, captures_t>,
            "There is no capture with the name requested.");
        constexpr std::size_t index = detail::index_of_capture<capture_name<Name...>, captures_t>;
        return std::get<index>(this->captures).result(allocator);
    }

    constexpr std::size_t mark_count() const {
        return std::tuple_size_v<captures_t>;
    }

    template <typename... Nodes>
    auto operator()(Nodes&&... nodes) const {
        return static_cast<const Derived*>(this)->replace_children(nodes...);
    }
};

} // namespace md
