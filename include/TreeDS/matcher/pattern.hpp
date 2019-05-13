#pragma once

#include <TreeDS/basic_tree.hpp>

namespace md {

template <typename PatternTree>
class pattern {

    protected:
    PatternTree pattern_tree;

    public:
    pattern(PatternTree&& tree) :
            pattern_tree(tree) {
    }

    template <typename T, typename Node, typename Policy, typename Allocator>
    bool match(basic_tree<T, Node, Policy, Allocator>& tree) {
        return this->pattern_tree.match_node(tree.get_root());
    }

    template <typename T, typename Node, typename Policy, typename Allocator>
    void assign_result(tree<T, Node, Policy, Allocator>& tree) {
        tree = pattern_tree.get_matched_node(tree.allocator);
    }

    template <std::size_t index, typename T, typename Node, typename Policy, typename Allocator>
    void assign_capture(tree<T, Node, Policy, Allocator>& tree) {
        tree = pattern_tree.template get_captured_node<index>(tree.allocator);
    }

    std::size_t size() const {
        return this->pattern_tree.capture_size();
    }
};

} // namespace md
