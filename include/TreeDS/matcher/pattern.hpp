#pragma once

#include <TreeDS/basic_tree.hpp>
#include <TreeDS/matcher/node/matcher.hpp>

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
        pattern_tree.reset();
        return this->pattern_tree.match_node(tree.root, tree.get_node_allocator());
    }

    template <typename T, typename Node, typename Policy, typename Allocator>
    bool match(const basic_tree<T, Node, Policy, Allocator>& tree) {
        pattern_tree.reset();
        return this->pattern_tree.match_node(tree.root, tree.get_node_allocator());
    }

    template <typename T, typename Node, typename Policy, typename Allocator>
    void assign_result(tree<T, Node, Policy, Allocator>& tree) {
        tree = pattern_tree.get_matched_node(tree.allocator);
    }

    template <std::size_t Index, typename T, typename Node, typename Policy, typename Allocator>
    void assign_capture(capture_index<Index> index, tree<T, Node, Policy, Allocator>& tree) {
        tree = pattern_tree.get_captured_node(index, tree.allocator);
    }

    template <char... Name, typename T, typename Node, typename Policy, typename Allocator>
    void assign_capture(capture_name<Name...> name, tree<T, Node, Policy, Allocator>& tree) {
        tree = pattern_tree.get_captured_node(name, tree.allocator);
    }

    std::size_t size() const {
        return this->pattern_tree.capture_size();
    }
};

} // namespace md
