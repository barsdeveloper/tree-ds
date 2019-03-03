#pragma once

#include <memory> // std::allocator

namespace md::detail {

template <typename ActualPolicy, typename Node, typename Allocator>
class basic_policy {

    protected:
    const Node* root    = nullptr;
    const Node* current = nullptr;
    Allocator allocator {};

    public:
    constexpr basic_policy() {
    }

    /*
     * Create a policy that does not point at the beginning of the iteration but somewhere into the range. This means
     * that we must reconstruct the state like if we started at the beginning and arrived at current. Current can also
     * be nullptr, this means the policy pointes at the beginning of the sequence.
     */
    basic_policy(const Node* root, const Node* current, const Allocator& allocator) :
            root(root),
            current(current),
            allocator(allocator) {
    }

    public:
    const Node* get_root() const {
        return this->root;
    }

    const Node* get_current_node() const {
        return this->current;
    }

    const Allocator& get_allocator() const {
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

    void update(const Node& current, const Node* replacement) {
        if (&current == this->current) {
            this->current = replacement;
        }
    }
};

/*
 * Tag classes are expected to provide two "get_instance" methods. A simple one that creates a policy which points at
 * at the beginning of a sequence and one that creates a policy that points somewhere in the range.
 */
template <template <typename, typename> class ActualPolicyType>
struct tag {
    template <typename Node, typename Allocator>
    ActualPolicyType<Node, Allocator> get_instance(
        const Node* root,
        const Node* current,
        const Allocator& allocator) const {
        return {root, current, allocator};
    }
};

} // namespace md::detail
