#pragma once

#include <cstddef>     // std::size_t
#include <type_traits> // std::is_same_v, std::is_convertible_v
#include <utility>     // std::declval()

#include <TreeDS/matcher/utility.hpp>
#include <TreeDS/matcher/value/alternative_match.hpp>
#include <TreeDS/matcher/value/product_match.hpp>
#include <TreeDS/matcher/value/true_matcher.hpp>
#include <TreeDS/node/struct_node.hpp>
#include <TreeDS/tree.hpp>

namespace md {

template <typename Derived, typename ValueMatcher, typename FirstChild, typename NextSibling>
class matcher : public struct_node_base<Derived, ValueMatcher, FirstChild, NextSibling> {

    /*   ---   FRIENDS   ---   */
    template <typename, typename, typename, typename>
    friend class matcher;

    /*   ---   CONSTANTS   ---   */
    static constexpr bool IS_CAPTURE = is_same_template<ValueMatcher, const_name<>>;

    /*   ---   TYPES   ---   */
    public:
    /// @brief Type tuple containing references to capturing matcher in this subtree
    using captures_t
        = decltype(
            std::tuple_cat(
                std::declval<std::conditional_t<IS_CAPTURE, std::tuple<matcher&>, std::tuple<>>>(),
                std::declval<typename detail::matcher_traits<matcher>::children_captures>(),
                std::declval<typename detail::matcher_traits<matcher>::siblings_captures>()));

    /*   ---   VALIDATION   ---   */
    static_assert(
        !(
            // This capture has a (non empty) name
            detail::is_const_name<ValueMatcher>
            // There exists another capture among children with the same name
            && detail::is_valid_name<ValueMatcher, typename detail::matcher_traits<matcher>::children_captures>),
        "Named captures must have unique names.");

    /*   ---   ATTRIBUTES   ---   */
    protected:
    const void* matched_node = nullptr;
    std::size_t steps        = 0u;
    captures_t captures      = this->get_following_captures();
    std::array<const void*, matcher::children()> child_match_attempt_begin{};

    /*   ---   CONSTRUCTORS   ---   */
    public:
    using struct_node_base<Derived, ValueMatcher, FirstChild, NextSibling>::struct_node_base;

    /*
     * This copy constructor is needed because the implicitly generated one would rewrite the attribute captures (which
     * stores references) using the values from other. Instead we need that attribute to be calculated every time a new
     * object is constructed. This is not a problem since this calculation happens at compile time.
     */
    matcher(const matcher& other) :
            struct_node_base<Derived, ValueMatcher, FirstChild, NextSibling>(other),
            matched_node(other.matched_node) {
    }

    /*   ---   METHODS   ---   */
    private:
    constexpr auto get_following_captures() {
        return std::tuple_cat(this->get_this_capture(), this->get_child_capture(), this->get_sibling_capture());
    }

    constexpr auto get_this_capture() {
        if constexpr (is_same_template<ValueMatcher, const_name<>>) {
            return std::tie(*this);
        } else {
            return std::make_tuple();
        }
    }

    constexpr auto get_child_capture() {
        if constexpr (matcher::has_first_child()) {
            return this->first_child.get_following_captures();
        } else {
            return std::make_tuple();
        }
    }

    constexpr auto get_sibling_capture() {
        if constexpr (matcher::has_next_sibling()) {
            return this->next_sibling.get_following_captures();
        } else {
            return std::make_tuple();
        }
    }

    protected:
    constexpr static bool child_may_steal_node() {
        return Derived::info.shallow_matches_null
            && matcher::foldl_children_types( // Count the children that do NOT match null
                   [](auto&& accumulated, auto&& element) {
                       using type = typename std::decay_t<decltype(element)>::type;
                       return accumulated + (type::info.matches_null ? 0u : 1u);
                   },
                   0u)
            <= 1u; // If at most one child requires a node, then one of the children may steal the target of its parent
    }

    template <typename NodeAllocator>
    bool did_child_steal_node(unique_ptr_alloc<NodeAllocator>& result, NodeAllocator& allocator) {
        if constexpr (matcher::child_may_steal_node()) {
            unique_ptr_alloc<NodeAllocator> ptr;
            if (this->foldl_children(
                    [&](bool accumulated, auto& child) {
                        if (child.get_matched_node(allocator) == this->get_matched_node(allocator)) {
                            ptr = child.result(allocator);
                            return true;
                        }
                        return accumulated;
                    },
                    false)) {
                result = std::move(ptr);
                return true;
            }
        }
        return false;
    }

    template <typename Value>
    bool match_value(const Value& value) {
        if constexpr (std::is_same_v<ValueMatcher, true_matcher>) {
            return true;
        } else if constexpr (is_same_template<ValueMatcher, product_match<std::nullptr_t, std::nullptr_t>>) {
            return this->value.match_value(value);
        } else {
            return this->value == value;
        }
    }

    public:
    bool empty() const {
        return this->matched_node == nullptr;
    }

    void reset() {
        this->matched_node = nullptr;
        if constexpr (matcher::has_first_child()) {
            this->first_child.reset();
        }
        if constexpr (matcher::has_next_sibling()) {
            this->next_sibling.reset();
        }
    }

    template <typename NodeAllocator>
    allocator_value_type<NodeAllocator>* get_child_match_attempt_begin(std::size_t index, const NodeAllocator&) const {
        return static_cast<allocator_value_type<NodeAllocator>*>(this->child_match_attempt_begin[index]);
    }

    template <typename NodeAllocator>
    allocator_value_type<NodeAllocator>* get_matched_node(NodeAllocator&) const {
        return static_cast<allocator_value_type<NodeAllocator>*>(const_cast<void*>(this->matched_node));
    }

    template <typename NodeAllocator>
    unique_ptr_alloc<NodeAllocator> clone_matched_node(NodeAllocator& allocator) const {
        return allocate(allocator, this->get_matched_node(allocator)->get_value());
    }

    template <
        typename NodeAllocator,
        typename Iterator,
        typename AckowledgeFunction = detail::empty_t,
        typename RematchFunction    = detail::empty_t>
    bool search_node_this(
        NodeAllocator& allocator,
        Iterator& it,
        AckowledgeFunction&& ackowledge = detail::empty_t()) {
        constexpr matcher_info_t info = Derived::info;
        using node_ptr                = const allocator_value_type<NodeAllocator>*;
        // If it can and prefers to match nothing
        if constexpr (info.prefers_null) {
            return true;
        }
        Iterator begin = it;
        // Try to match something
        while (it) {
            ++this->steps;
            node_ptr candidate = it.get_raw_node(); // save current node because it may be modified by search_node_impl
            if (this->cast()->search_node_impl(allocator, it)) {
                this->matched_node = candidate;
                if constexpr (!is_empty<AckowledgeFunction>) {
                    ackowledge(candidate);
                }
                ++it;
                return true;
            }
            ++it;
        }
        // Couldn't match anything, but it still may match nothing
        if constexpr (info.matches_null) {
            it = begin;
            return true;
        }
        return false;
    }

    template <
        typename NodeAllocator,
        typename Iterator,
        typename AckowledgeFunction = detail::empty_t,
        typename RematchFunction    = detail::empty_t>
    bool search_node_child(
        NodeAllocator& allocator,
        Iterator&& it,
        AckowledgeFunction&& ackowledge = detail::empty_t(),
        RematchFunction&& rematch       = detail::empty_t()) {
        if constexpr (matcher::has_first_child()) {
            if (!this->get_first_child().search_node(allocator, it, ackowledge, rematch)) {
                return false;
            }
        }
        return true;
    }

    template <
        typename NodeAllocator,
        typename Iterator,
        typename AckowledgeFunction = detail::empty_t,
        typename RematchFunction    = detail::empty_t>
    bool search_node_sibling(
        NodeAllocator& allocator,
        Iterator& it,
        AckowledgeFunction&& ackowledge = detail::empty_t(),
        RematchFunction&& rematch       = detail::empty_t()) {
        using it_t = std::decay_t<Iterator>;
        if constexpr (matcher::has_next_sibling()) {
            it_t begin = it;
            if (this->get_next_sibling().search_node(allocator, it, ackowledge, rematch)) {
                return true;
            } else {
                if constexpr (!is_empty<std::decay_t<RematchFunction>>) {
                    it_t search_start = begin;
                    it                = search_start; // Go back to where we started
                    while (it && rematch(*this, it)) {
                        search_start = it;
                        if (!this->cast()->search_node_this(allocator, it, ackowledge)) {
                            continue; // Rematched it doesn't satisfy this matcher
                        }
                        if (this->cast()->get_next_sibling().search_node(allocator, it, ackowledge, rematch)) {
                            return true; // Rematched it satisfies the next sibling
                        }
                        it = search_start;
                    }
                }
                if (Derived::info.matches_null && !this->empty()) {
                    it = it.other_node(this->get_matched_node(allocator)); // This matched renonunces to its node
                    this->reset();
                    return this->cast()->get_next_sibling().search_node(allocator, it, ackowledge, rematch);
                }
            }
            return false;
        }
        return true;
    }

    template <
        typename NodeAllocator,
        typename Iterator,
        typename AckowledgeFunction = detail::empty_t,
        typename RematchFunction    = detail::empty_t>
    bool search_node(
        NodeAllocator&& allocator,
        Iterator&& it,
        AckowledgeFunction&& ackowledge = detail::empty_t(),
        RematchFunction&& rematch       = detail::empty_t()) {
        if (this->cast()->search_node_this(allocator, it, ackowledge)) {
            return this->cast()->search_node_sibling(allocator, it, ackowledge, rematch);
        }
        return false;
    }

    template <typename NodeAllocator>
    // Derived may want to rewrite this
    bool match_node_shallow(allocator_value_type<NodeAllocator>* node, NodeAllocator& allocator) {
        return this->match_node(node, allocator);
    }

    template <typename NodeAllocator>
    unique_ptr_alloc<NodeAllocator> result(NodeAllocator& allocator) {
        if (!this->matched_node) {
            return nullptr;
        }
        return this->cast()->result_impl(allocator);
    }

    template <std::size_t Index, typename NodeAllocator>
    unique_ptr_alloc<NodeAllocator> marked_result(const_index<Index>, NodeAllocator& allocator) {
        static_assert(Index - 1 < std::tuple_size_v<captures_t>, "There is no capture with the index requested.");
        return std::get<Index - 1>(this->captures).result(allocator);
    }

    template <char... Name, typename NodeAllocator>
    unique_ptr_alloc<NodeAllocator> marked_result(const_name<Name...>, NodeAllocator& allocator) {
        static_assert(
            sizeof...(Name) > 0,
            "Capture's name must be not empty. For example const_name<'a'> is OK, while const_name<> is not.");
        static_assert(
            detail::is_valid_name<const_name<Name...>, captures_t>,
            "There is no capture with the name requested.");
        constexpr std::size_t index = detail::index_of_capture<const_name<Name...>, captures_t>;
        return std::get<index>(this->captures).result(allocator);
    }

    constexpr std::size_t mark_count() const {
        return std::tuple_size_v<captures_t>;
    }
};

} // namespace md
