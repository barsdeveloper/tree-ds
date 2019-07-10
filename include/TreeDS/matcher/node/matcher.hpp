#pragma once

#include <cstddef>     // std::size_t
#include <type_traits> // std::is_same_v, std::is_convertible_v

#include <TreeDS/matcher/utility.hpp>
#include <TreeDS/matcher/value/true_matcher.hpp>
#include <TreeDS/node/struct_node.hpp>
#include <TreeDS/tree.hpp>

namespace md {

enum class quantifier {
    DEFAULT,
    RELUCTANT,
    GREEDY,
    POSSESSIVE
};

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
    void* target_node   = nullptr;
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
    std::array<void*, sizeof...(Children)> child_match_attempt_begin {};

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
    private:
    template <typename Iterator, typename MatchFunction>
    bool try_match(Iterator& it, const MatchFunction& match, std::size_t index, const matcher_info_t& info) {
        if (!this->child_match_attempt_begin[index]) {
            this->child_match_attempt_begin[index] = it.get_current_node();
        }
        while (it) {
            if (apply_at_index(
                    [&](auto& child) {
                        return match(it, child);
                    },
                    this->children,
                    index)) {
                it.increment();
                return true;
            }
            it.increment();
        }
        if (info.matches_null && this->child_match_attempt_begin[index]) {
            it = Iterator(it, static_cast<decltype(it.get_current_node())>(this->child_match_attempt_begin[index]));
            return true;
        }
        return false;
    }

    protected:
    template <typename Node>
    static auto get_children_supplier(Node& target) {
        return [node = target.get_first_child()]() mutable -> Node* {
            Node* result = node;
            node         = node->get_next_sibling();
            return result;
        };
    }

    static constexpr std::size_t child_steal_index() {
        if constexpr (matcher::child_may_steal_target()) {
            std::size_t result = 0;
            std::size_t i      = 0;
            (..., (!Children::info.matches_null ? result = i++ : i++));
            return result;
        } else {
            return sizeof...(Children);
        }
    }

    // True if no more than one children requires a node in order to match
    static constexpr bool child_may_steal_target() {
        if constexpr (Derived::info.shallow_matches_null) {
            std::size_t requires_match = 0;
            (..., (requires_match += !Children::info.matches_null ? 1u : 0u));
            return requires_match == 1;
        } else {
            return false;
        }
    }

    const matcher_info_t& get_child_info(std::size_t index) const {
        return apply_at_index(
            [](auto& child) -> const matcher_info_t& { return child.info; },
            this->children,
            index);
    }

    template <typename NodeAllocator>
    bool let_child_steal(allocator_value_type<NodeAllocator>& node, NodeAllocator& allocator) {
        if constexpr (matcher::child_may_steal_target()) {
            // Children will steal this node because it cannot be matched using this matcher
            return apply_at_index(
                [&](auto& child) -> bool {
                    return child.match_node(&node, allocator);
                },
                this->children,
                matcher::child_steal_index());
        } else {
            return false;
        }
    }

    template <typename NodeAllocator>
    bool did_child_steal_target(unique_node_ptr<NodeAllocator>& result, NodeAllocator& allocator) {
        if constexpr (matcher::child_may_steal_target()) {
            unique_node_ptr<NodeAllocator> ptr;
            std::apply(
                [&](auto&... children) {
                    ((children.get_node(allocator) == this->get_node(allocator)
                          ? ptr = children.result(allocator)
                          : ptr),
                     ...);
                },
                this->children);
            if (ptr != nullptr) {
                result = std::move(ptr);
                return true;
            }
        }
        return false;
    }

    template <typename NodeAllocator>
    allocator_value_type<NodeAllocator>* get_child_match_attempt_begin(std::size_t index, const NodeAllocator&) const {
        return static_cast<allocator_value_type<NodeAllocator>*>(this->child_match_attempt_begin[index]);
    }

    template <typename Value>
    bool match_value(const Value& value) {
        if constexpr (std::is_same_v<ValueMatcher, true_matcher>) {
            return true;
        } else {
            return this->value == value;
        }
    }

    template <typename Allocator, typename Iterator, typename MatchFunction, typename RematchFunction = std::nullptr_t>
    bool match_children(
        Allocator& allocator,
        Iterator it,
        const MatchFunction& match,
        const RematchFunction& rematch = nullptr) {
        if constexpr (matcher::children_count() > 0) {
            int current_child;
            for (current_child = 0; current_child < static_cast<int>(this->children_count()); ++current_child) {
                const matcher_info_t& info = this->get_child_info(current_child);
                if (info.matches_null && (info.reluctant || !it)) {
                    // If current chiuld prefers to not match anything
                    continue;
                }
                if (!this->try_match(it, match, current_child, info)) {
                    if constexpr (std::is_same_v<RematchFunction, std::nullptr_t>) {
                        // There isn't any rematch function
                        return false;
                    } else {
                        auto do_rematch = [&](auto& child) -> bool {
                            if (rematch(it, child)) {
                                return true;
                            } else if (child.info.matches_null && !child.empty()) {
                                it = Iterator(it, this->get_child_match_attempt_begin(current_child, allocator));
                                child.drop_target();
                                this->child_match_attempt_begin[child.get_index()] = nullptr;
                                return true;
                            }
                            return false;
                        };
                        // There is a rematch function
                        while (current_child >= 0) {
                            if (apply_at_index(do_rematch, this->children, current_child)) {
                                break; // We found a child that could rematch and leave the other children new nodes
                            }
                            --current_child;
                        }
                        if (current_child < 0) {
                            return false;
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
        return this->target_node == nullptr;
    }

    template <typename NodeAllocator>
    allocator_value_type<NodeAllocator>* get_node(NodeAllocator&) const {
        return static_cast<allocator_value_type<NodeAllocator>*>(this->target_node);
    }

    template <typename NodeAllocator>
    unique_node_ptr<NodeAllocator> clone_node(NodeAllocator& allocator) const {
        return allocate(allocator, this->get_node(allocator)->get_value());
    }

    void drop_target() {
        this->target_node = nullptr;
    }

    void reset() {
        this->target_node = nullptr;
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
            this->target_node = node;
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
        if (this->target_node == nullptr) {
            return nullptr;
        }
        return static_cast<Derived*>(this)->result_impl(allocator);
    }

    template <std::size_t Index, typename NodeAllocator>
    unique_node_ptr<NodeAllocator> marked_result(capture_index<Index>, NodeAllocator& allocator) {
        static_assert(Index - 1 < std::tuple_size_v<captures_t>, "There is no capture with the index requested.");
        return std::get<Index - 1>(this->captures).result(allocator);
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
}; // namespace md

} // namespace md
