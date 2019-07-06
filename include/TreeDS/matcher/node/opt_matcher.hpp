#pragma once

#include <functional> // std::reference_wrapper
#include <optional>

#include <TreeDS/allocator_utility.hpp>
#include <TreeDS/matcher/node/matcher.hpp>
#include <TreeDS/matcher/pattern.hpp>
#include <TreeDS/matcher/value/true_matcher.hpp>
#include <TreeDS/policy/siblings.hpp>

namespace md {

template <quantifier Quantifier, typename ValueMatcher, typename... Children>
class opt_matcher : public matcher<opt_matcher<Quantifier, ValueMatcher, Children...>, ValueMatcher, Children...> {

    /*   ---   FRIENDS   ---   */
    template <typename, typename, typename...>
    friend class matcher;

    /*   ---   ATTRIBUTES   ---   */
    public:
    static constexpr matcher_info_t info {
        // Matches null
        (... && Children::info.matches_null),
        // It matches null if we consider just this node
        true,
        // Reluctant behavior
        Quantifier == quantifier::RELUCTANT};

    /*   ---   CONSTRUCTOR   ---   */
    using matcher<opt_matcher, ValueMatcher, Children...>::matcher;

    /*   ---   METHODS   ---   */
    template <typename NodeAllocator>
    bool match_node_impl(allocator_value_type<NodeAllocator>& node, NodeAllocator& allocator) {
        if (!this->match_value(node.get_value())) {
            return this->let_child_steal(node, allocator);
        }
        auto target = node.get_first_child();
        // Match children of the pattern
        auto do_match_child = [&](auto& child) -> bool {
            auto* current = target;
            if (child.match_node(current, allocator)) {
                if (current) {
                    target = current->get_next_sibling();
                }
                return true;
            }
            while (current != nullptr) {
                current = current->get_next_sibling();
                if (child.match_node(current, allocator)) {
                    return true;
                }
            }
            return child.info.matches_null;
        };
        return this->match_children(do_match_child);
    }

    template <typename NodeAllocator>
    unique_node_ptr<NodeAllocator> result_impl(NodeAllocator& allocator) {
        unique_node_ptr<NodeAllocator> result = this->clone_node(allocator);
        auto attach_child                     = [&](auto& child) {
            if (!child.empty()) {
                result->assign_child_like(
                    child.result(allocator),
                    *child.get_node(allocator));
            }
        };
        std::apply(
            [&](auto&... children) {
                (..., attach_child(children));
            },
            this->children);
        return std::move(result);
    }

    template <typename... Nodes>
    constexpr opt_matcher<Quantifier, ValueMatcher, Nodes...> replace_children(Nodes&... nodes) const {
        return {this->value, nodes...};
    }
};

template <quantifier Quantifier = quantifier::DEFAULT, typename ValueMatcher>
opt_matcher<Quantifier, ValueMatcher> opt(const ValueMatcher& value_matcher) {
    return {value_matcher};
}
template <quantifier Quantifier = quantifier::DEFAULT>
opt_matcher<Quantifier, true_matcher> opt() {
    return {true_matcher()};
}

} // namespace md
