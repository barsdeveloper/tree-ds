#pragma once

#include <any>
#include <list>
#include <numeric> // std::iota
#include <tuple>   // std::apply
#include <vector>

#include <TreeDS/matcher/node/matcher.hpp>
#include <TreeDS/matcher/value/true_matcher.hpp>
#include <TreeDS/node/binary_node.hpp>
#include <TreeDS/node/struct_node.hpp>

namespace md {

//   ---   Matcher class   ---
template <
    typename Node,
    typename ValueMatcher,
    typename... Children>
class any_match : public matcher<Node, ValueMatcher, Children...> {

    protected:
    /// @brief Horizontal cut of the tree where the non matched region starts. Children will start matching from there.
    std::vector<Node*> frontier {};

    using matcher<Node, ValueMatcher, Children...>::matcher;

    private:
    void calculate_frontier(const Node& from) {
        if (from.get_value().accept(this->get_value_matcher())) {
            Node* pointer = from.get_first_child();
            while (pointer != nullptr) {
                calculate_frontier(*pointer);
                pointer = pointer->get_next_sibling();
            }
        } else {
            frontier.push_back(&from);
        }
    }

    public:
    bool match_node(const Node& node) {
        calculate_frontier(node);
        if constexpr (sizeof...(Children) > 0) {
            std::list<unsigned> to_match(frontier.size());
            std::iota(to_match.begin(), to_match.end(), 0); // to_match is 0..frontier.size() now
            auto match_child = [&](auto& node) -> bool {
                for (auto it = to_match.begin(); it != to_match.end(); ++it) {
                    unsigned index = *it;
                    if (node.match_node(frontier[index])) {
                        it            = to_match.erase(it);
                        this->matched = &frontier[index];
                        return true;
                    }
                }
                return false;
            };
            return std::apply(
                [&](auto&... children) {
                    return (... && match_node(children));
                },
                this->get_children());
        }
    }
};

//   ---   Supplier function   ---
template <typename Node, typename ValueMatcher = true_matcher>
any_match<Node, ValueMatcher> any(
    std::optional<std::reference_wrapper<Node*>> target = std::nullopt,
    const ValueMatcher& value_matcher                   = ValueMatcher()) {
    any_match<Node, ValueMatcher> matcher(value_matcher);
    matcher.matched = target;
}

} // namespace md
