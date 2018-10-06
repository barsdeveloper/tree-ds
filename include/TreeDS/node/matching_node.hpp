#pragma once

#include <TreeDS/match/permissive_matcher.hpp>
#include <TreeDS/node/struct_node.hpp>

namespace ds {

template <typename Matcher = permissive_matcher, typename... Children>
class matching_node : public struct_node<Matcher, Children...> {

    protected:
    Matcher argument_matcher{};

    public:
    constexpr matching_node(const Matcher& value, Children... children) :
            struct_node<Matcher, Children...>(value, children...) {
    }

    template <typename Node>
    bool match(const Node& node) {
    }
};

} // namespace ds
