#pragma once

#include <vector>

#include <TreeDS/matcher/node/node_matcher.hpp>
#include <TreeDS/matcher/value/true_matcher.hpp>

namespace ds {

template <typename Node, typename ValueMatcher = true_matcher, typename... Children>
class any_position_match : public node_matcher<ValueMatcher, Children...> {

    protected:
    /// @brief Horizontal cut of the tree where the non matched region starts. Children will start matching from there.
    std::vector<Node*> frontier{};

    public:
    using node_matcher<ValueMatcher, Children...>::node_matcher;

    void calculate_frontier(const Node& from) {
        if (this->match_value(from.get_value())) {
            Node* pointer = from.get_first_child();
            while (pointer != nullptr) {
                calculate_frontier(*pointer);
                pointer = pointer->get_next_sibling();
            }
        } else {
            frontier.push_back(&from);
        }
    }

    bool match_node(const Node& node) {
        calculate_frontier(node);
        if constexpr (sizeof...(Children) > 0) {
            auto match_child = [&](auto& first_child, int index, auto&... other_children) -> bool {
                if (first_child.match_node(frontier[index])) {
                    if constexpr (sizeof...(other_children) > 0) {
                        return match_child(index + 1, other_children...);
                    }
                }
                if (sizeof...(other) > frontier.size() - index) {
                    return false;
                }
            };
        }
    }
};

} // namespace ds
