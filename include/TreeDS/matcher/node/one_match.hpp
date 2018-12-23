#pragma

#include <functional> // std::reference_wrapper
#include <optional>

#include <TreeDS/matcher/node/matcher.hpp>

namespace md {

//   ---   Matcher class   ---
template <typename Node, typename ValueMatcher = true_matcher, typename... Children>
class one_match : matcher<Node, ValueMatcher, Children...> {

    using matcher<Node, ValueMatcher, Children...>::matcher;

    public:
    bool match_node(const Node& node) {
        return node.get_value().accept(this->get_value_matcher());
    }
};

//   ---   Supplier function   ---
template <typename Node, typename ValueMatcher = true_matcher>
one_match<Node, ValueMatcher> one(
    std::optional<std::reference_wrapper<Node*>> target = std::nullopt,
    const ValueMatcher& value_matcher                   = ValueMatcher()) {
    return {value_matcher, target};
}

} // namespace md
