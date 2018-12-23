#pragma once

#include <functional> // std::reference_wrapper
#include <optional>

#include <TreeDS/matcher/value/true_matcher.hpp>
#include <TreeDS/node/struct_node.hpp>

namespace md {

template <
    typename Node,
    typename ValueMatcher = true_matcher,
    typename... Children>
class matcher : public struct_node<ValueMatcher, Children...> {
    protected:
    std::optional<std::reference_wrapper<Node*>> matched = std::nullopt;

    using struct_node<ValueMatcher, Children...>::struct_node;

    matcher(std::reference_wrapper<Node*> matched_node, const ValueMatcher& value_matcher) :
            struct_node<ValueMatcher, Children...>(value_matcher),
            matched(matched_node) {
    }

    ValueMatcher& get_value_matcher() {
        return this->get_value();
    }

    template <typename Value>
    bool match_value(const Value& value) {
        return get_value_matcher().match_value(value);
    }
};

} // namespace md
