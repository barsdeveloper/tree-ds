#pragma once

#include <TreeDS/matcher/matchable.hpp>
#include <TreeDS/matcher/node/node_matcher.hpp>
#include <TreeDS/matcher/value/permissive_matcher.hpp>
#include <TreeDS/node/struct_node.hpp>

namespace ds {

template <typename Node, typename ValueMatcher = permissive_matcher, typename... Children>
class node_matcher : public struct_node<ValueMatcher, Children...> {

    protected:
    ValueMatcher& get_value_matcher() {
        return this->value;
    }

    bool match_value(const matchable& target_value) {
        return get_value_matcher().match_value(target_value);
    }

    public:
    constexpr node_matcher(const ValueMatcher& value_matcher, Children... children) :
            struct_node<node_matcher, Children...>(value_matcher, children...) {
    }
};

} // namespace ds
