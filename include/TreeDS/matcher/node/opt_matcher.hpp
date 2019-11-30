#pragma once

#include <functional> // std::reference_wrapper
#include <optional>

#include <TreeDS/allocator_utility.hpp>
#include <TreeDS/matcher/node/matcher.hpp>
#include <TreeDS/matcher/pattern.hpp>
#include <TreeDS/matcher/value/true_matcher.hpp>
#include <TreeDS/policy/siblings.hpp>

namespace md {

template <quantifier Quantifier, typename ValueMatcher, typename FirstChild, typename NextSibling>
class opt_matcher : public matcher<
                        opt_matcher<Quantifier, ValueMatcher, FirstChild, NextSibling>,
                        ValueMatcher,
                        FirstChild,
                        NextSibling> {

    /*   ---   PARAMETERS   ---   */
    // It matches null only if all its children do so
    static constexpr bool MATCHES_NULL = opt_matcher::foldl_children_types(
        [](auto&& accumulator, auto&& element) {
            using element_type = typename std::remove_reference_t<decltype(element)>::type;
            return accumulator && element_type::info.matches_null;
        },
        true);

    /*   ---   ATTRIBUTES   ---   */
    public:
    static constexpr matcher_info_t info {
        // Matches null
        MATCHES_NULL,
        // Shallow matches null
        true,
        // Prefers null
        MATCHES_NULL && (Quantifier == quantifier::RELUCTANT),
        // Possessive
        Quantifier == quantifier::POSSESSIVE};

    /*   ---   CONSTRUCTOR   ---   */
    using matcher<opt_matcher, ValueMatcher, FirstChild, NextSibling>::matcher;

    /*   ---   METHODS   ---   */
    template <typename NodeAllocator, typename Iterator>
    bool search_node_impl(NodeAllocator& allocator, Iterator& it) {
        if (!this->match_value(*it)) {
            if constexpr (opt_matcher::child_may_steal_target()) {
                auto fixed_it = it.other_policy(policy::fixed());
                return this->search_node_child(allocator, fixed_it);
            } else {
                return false;
            }
        }
        using basic_navigator = node_navigator<typename Iterator::node_type*>;
        auto target_it
            = it
                  .other_policy(policy::siblings())
                  .other_navigator(basic_navigator())
                  .go_first_child();
        // Match children of the pattern
        bool result = this->search_node_child(allocator, std::move(target_it));
        if constexpr (
            !opt_matcher::info.possessive
            && opt_matcher::child_may_steal_target()) {
            // Every other match failed, we try the last possibility: one of the children matches this node
            if (!result) {
                return this->search_node_child(allocator, it.other_policy(policy::fixed()));
            }
        }
        return result;
    }

    template <typename NodeAllocator>
    unique_ptr_alloc<NodeAllocator> result_impl(NodeAllocator& allocator) {
        unique_ptr_alloc<NodeAllocator> result;
        if (this->did_child_steal_target(result, allocator)) {
            return std::move(result);
        }
        // Assign here because did_child_stea_target assignins result
        result = this->clone_node(allocator);
        this->foldl_children(
            [&](bool, auto& child) {
                if (!child.empty()) {
                    result->assign_child_like(child.result(allocator), *child.get_node(allocator));
                }
                return true;
            },
            true);
        return std::move(result);
    }

    template <typename... Nodes>
    constexpr opt_matcher<Quantifier, ValueMatcher, Nodes...> replace_children(Nodes&... nodes) const {
        return {this->value, nodes...};
    }

    template <typename Child>
    constexpr opt_matcher<Quantifier, ValueMatcher, std::remove_reference_t<Child>, NextSibling>
    with_first_child(Child&& child) const {
        return {this->value, child, this->next_sibling};
    }

    template <typename Sibling>
    constexpr opt_matcher<Quantifier, ValueMatcher, FirstChild, std::remove_reference_t<Sibling>>
    with_next_sibling(Sibling&& sibling) const {
        return {this->value, this->first_child, sibling};
    }
};

template <quantifier Quantifier = quantifier::DEFAULT, typename ValueMatcher>
opt_matcher<Quantifier, ValueMatcher, detail::empty_t, detail::empty_t> opt(const ValueMatcher& value_matcher) {
    return {value_matcher, detail::empty_t(), detail::empty_t()};
}
template <quantifier Quantifier = quantifier::DEFAULT>
opt_matcher<Quantifier, true_matcher, detail::empty_t, detail::empty_t> opt() {
    return {true_matcher(), detail::empty_t(), detail::empty_t()};
}

} // namespace md
