#pragma once

#include <memory> // std::allocator

#include <TreeDS/utility.hpp>

namespace md::detail {

template <
    typename ActualPolicy,
    typename Node,
    typename Allocator>
class basic_policy {

    protected:
    using node_type      = Node;
    using allocator_type = Allocator;

    protected:
    node_type* root    = nullptr;
    node_type* current = nullptr;
    allocator_type allocator {};

    public:
    basic_policy() {
    }

    /*
     * Create a policy that does not point at the beginning of the iteration but somewhere into the range. This means
     * that we must reconstruct the state like if we started at the beginning and arrived at current. Current can also
     * be nullptr, this means the policy pointes at the beginning of the sequence.
     */
    basic_policy(Node* root, Node* current, const Allocator& allocator) :
            root(root),
            current(current),
            allocator(allocator) {
    }

    template <
        typename OtherActualPolicy,
        typename OtherNode,
        typename = std::enable_if_t<is_same_template<ActualPolicy, OtherActualPolicy>>,
        typename = std::enable_if_t<std::is_same_v<std::decay_t<OtherNode>, std::decay_t<Node>>>>
    basic_policy(
        const basic_policy<OtherActualPolicy, OtherNode, Allocator>& other,
        const Node* current = nullptr) :
            basic_policy(
                const_cast<Node*>(other.get_root()),
                const_cast<Node*>(current != nullptr ? current : other.get_current_node()),
                other.get_allocator()) {
    }

    public:
    node_type* get_root() const {
        return this->root;
    }

    node_type* get_current_node() const {
        return this->current;
    }

    const allocator_type& get_allocator() const {
        return this->allocator;
    }

    void increment() {
        this->current = static_cast<ActualPolicy*>(this)->increment_impl();
    }

    void decrement() {
        this->current = static_cast<ActualPolicy*>(this)->decrement_impl();
    }

    void go_first() {
        this->current = static_cast<ActualPolicy*>(this)->go_first_impl();
    }

    void go_last() {
        this->current = static_cast<ActualPolicy*>(this)->go_last_impl();
    }

    void update(Node& current, Node* replacement) {
        // Subclasses can override this in roder to manage their status.
        if (&current == this->current) {
            this->current = replacement;
        }
    }
};

/*
 * Tag classes are expected to provide two "get_instance" methods. A simple one that creates a policy which points at
 * at the beginning of a sequence and one that creates a policy that points somewhere in the range.
 */
template <template <typename...> class PolicyTemplate>
struct policy_tag {
    template <
        typename Node,
        typename Allocator>
    PolicyTemplate<Node, Allocator> get_instance(
        Node* root,
        Node* current,
        const Allocator& allocator) const {
        return {root, current, allocator};
    }
};

} // namespace md::detail
