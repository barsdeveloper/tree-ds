#pragma once

#include <memory> // std::allocator

namespace md::detail {

template <typename ActualPolicy, typename Node, typename Allocator>
class basic_policy {

    protected:
    Node* root    = nullptr;
    Node* current = nullptr;
    Allocator allocator {};

    public:
    constexpr basic_policy() {
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

    public:
    Node* get_root() const {
        return this->root;
    }

    Node* get_current_node() const {
        return this->current;
    }

    const Allocator& get_allocator() const {
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
struct tag {
    template <typename Node, typename Allocator>
    PolicyTemplate<Node, Allocator> get_instance(
        Node* root,
        Node* current,
        const Allocator& allocator) const {
        return {root, current, allocator};
    }
};

} // namespace md::detail
