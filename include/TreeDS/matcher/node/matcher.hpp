#pragma once

#include <functional> // std::reference_wrapper
#include <optional>

#include <TreeDS/matcher/value/true_matcher.hpp>
#include <TreeDS/node/struct_node.hpp>
#include <TreeDS/tree.hpp>

namespace md {

template <
    template <typename, typename, typename...> class Derived,
    typename Node,
    typename ValueMatcher,
    std::size_t MinSiblings,
    std::size_t MaxSiblings,
    typename... Children>
class matcher : public struct_node<ValueMatcher, Children...> {

    protected:
    std::reference_wrapper<Node*> target_node = nullptr;

    public:
    matcher(const matcher&) = default;

    matcher(Node*& target_node, const ValueMatcher& value_matcher, Children&&... children) :
            struct_node<ValueMatcher, Children...>(value_matcher, std::forward<Children>(children)...),
            target_node(target_node) {
    }

    protected:
    bool match_node(Node* node) {
        this->target_node = nullptr;
        Node* current     = node;
        if (
            static_cast<
                Derived<Node, ValueMatcher, Children...>*>(this)
                /*
                 * Derived class is expected to have a bool match_impl(Node*) method. It is responsible to:
                 *   1) implement the actual logic to match a node,
                 *   2) call bool match_node(Node*) for each one of its children,
                 *   3) return true if itself and children matched correctly, false otherwise.
                 * Please note that node* passed to match_impl can also be a nullptr. Some matcher correctly accept it,
                 * for example any(..., true_matcher)
                 */
                ->match_impl(node)) {
            this->target_node = node;
            return true;
        }
        return false;
    }

    template <typename Value>
    bool match_value(const Value& value) {
        return this->value.match_value(value);
    }

    public:
    template <typename Tree>
    bool match_tree(Tree& tree) {
        if (tree.empty()) {
            return false;
        }
        return this->match_node(tree.get_root());
    }

    template <typename... Nodes>
    constexpr Derived<Node, ValueMatcher, Nodes...> operator()(Nodes&&... nodes) const {
        return {this->target_node, this->value, std::forward<Nodes>(nodes)...};
    }
};

} // namespace md
