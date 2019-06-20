#pragma once

#include <functional> // std::reference_wrapper
#include <optional>

#include <TreeDS/allocator_utility.hpp>
#include <TreeDS/matcher/node/matcher.hpp>
#include <TreeDS/matcher/pattern.hpp>
#include <TreeDS/matcher/value/true_matcher.hpp>
#include <TreeDS/policy/siblings.hpp>

namespace md {

template <typename ValueMatcher, typename... Children>
class one_matcher : public matcher<one_matcher<ValueMatcher, Children...>, ValueMatcher, Children...> {

    /*   ---   FRIENDS   ---   */
    template <typename, typename, typename...>
    friend class matcher;

    /*   ---   ATTRIBUTES   ---   */
    public:
    static constexpr matcher_info_t info {false, true};

    /*   ---   CONSTRUCTOR   ---   */
    using matcher<one_matcher, ValueMatcher, Children...>::matcher;

    /*   ---   METHODS   ---   */
    template <typename NodeAllocator>
    bool match_node_impl(allocator_value_type<NodeAllocator>& node, NodeAllocator& allocator) {
        if (!this->match_value(node.get_value())) {
            return false;
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
                    target = current->get_next_sibling();
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
    constexpr one_matcher<ValueMatcher, Nodes...> replace_children(Nodes&... nodes) const {
        return {this->value, nodes...};
    }
};

template <typename ValueMatcher>
one_matcher<ValueMatcher> one(const ValueMatcher& value_matcher) {
    return {value_matcher};
}

one_matcher<true_matcher> one() {
    return {true_matcher()};
}

} // namespace md
