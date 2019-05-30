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
        // Match children of the pattern
        using node_t           = allocator_value_type<NodeAllocator>;
        node_t* current_target = node.get_first_child();
        auto do_match_child    = [&](auto& child) {
            if (child.match_node(current_target, allocator)) {
                return true;
            }
            while (current_target != nullptr) {
                current_target = current_target->get_next_sibling();
                if (child.match_node(current_target, allocator)) {
                    return true;
                }
            }
            return false;
        };
        return this->match_children(do_match_child);
    }

    template <typename NodeAllocator>
    unique_node_ptr<NodeAllocator> get_matched_node_impl(NodeAllocator& allocator) {
        unique_node_ptr<NodeAllocator> result = allocate(
            allocator,
            static_cast<allocator_value_type<NodeAllocator>*>(this->target_node)->get_value());
        this->attach_matched_children(
            [&]() {
                return result.get();
            },
            allocator);
        return std::move(result);
    }

    template <typename NodeAllocator>
    void attach_matched_impl(allocator_value_type<NodeAllocator>& target, NodeAllocator& allocator) {
        target.shallow_copy_assign_child(
            *static_cast<allocator_value_type<NodeAllocator>*>(this->target_node),
            std::forward<NodeAllocator>(allocator));
    }

    template <typename... Nodes>
    constexpr one_matcher<ValueMatcher, Nodes...> with_children(Nodes&... nodes) const {
        return {this->value, nodes...};
    }
};

template <typename ValueMatcher>
one_matcher<ValueMatcher> one(const ValueMatcher& value_matcher) {
    return {value_matcher};
}

} // namespace md
