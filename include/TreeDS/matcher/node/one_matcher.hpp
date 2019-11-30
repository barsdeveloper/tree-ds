#pragma once

#include <functional> // std::reference_wrapper
#include <optional>

#include <TreeDS/allocator_utility.hpp>
#include <TreeDS/matcher/node/matcher.hpp>
#include <TreeDS/matcher/pattern.hpp>
#include <TreeDS/matcher/value/true_matcher.hpp>
#include <TreeDS/policy/siblings.hpp>

namespace md {

template <typename ValueMatcher, typename FirstChild, typename NextSibling>
class one_matcher : public matcher<
                        one_matcher<ValueMatcher, FirstChild, NextSibling>,
                        ValueMatcher,
                        FirstChild,
                        NextSibling> {

    /*   ---   ATTRIBUTES   ---   */
    public:
    static constexpr matcher_info_t info {
        // Matches null
        false,
        // Shallow matches null
        false,
        // Prefers null
        false,
        // Possessive
        true};

    /*   ---   CONSTRUCTORS   ---   */
    using matcher<one_matcher, ValueMatcher, FirstChild, NextSibling>::matcher;

    /*   ---   METHODS   ---   */
    template <typename NodeAllocator, typename Iterator>
    bool search_node_impl(NodeAllocator& allocator, Iterator& it) {
        if (!this->match_value(*it)) {
            return false;
        }
        using basic_navigator = node_navigator<decltype(it.get_raw_node())>;
        // Iterator that gives the children potential nodes to match
        auto target_it
            = it
                  .other_policy(policy::siblings())
                  .other_navigator(basic_navigator())
                  .go_first_child();
        return this->search_node_child(allocator, target_it);
    }

    template <typename NodeAllocator>
    unique_ptr_alloc<NodeAllocator> result_impl(NodeAllocator& allocator) {
        unique_ptr_alloc<NodeAllocator> node = this->clone_node(allocator);
        this->foldl_children(
            [&](bool, auto& child) {
                if (!child.empty()) {
                    node->assign_child_like(
                        child.result(allocator),
                        *child.get_node(allocator));
                }
                return true;
            },
            true);
        return std::move(node);
    }

    template <typename... Nodes>
    constexpr one_matcher<ValueMatcher, Nodes...>
    replace_children(Nodes&... nodes) const {
        return {this->value, nodes...};
    }

    template <typename Child>
    constexpr one_matcher<ValueMatcher, std::remove_reference_t<Child>, NextSibling>
    with_first_child(Child&& child) const {
        return {this->value, child, this->next_sibling};
    }

    template <typename Sibling>
    constexpr one_matcher<ValueMatcher, FirstChild, std::remove_reference_t<Sibling>>
    with_next_sibling(Sibling&& sibling) const {
        return {this->value, this->first_child, sibling};
    }
};

template <typename ValueMatcher>
one_matcher<ValueMatcher, detail::empty_t, detail::empty_t>
one(const ValueMatcher& value_matcher) {
    return {value_matcher, detail::empty_t(), detail::empty_t()};
}

one_matcher<true_matcher, detail::empty_t, detail::empty_t> one() {
    return {true_matcher(), detail::empty_t(), detail::empty_t()};
}

} // namespace md
