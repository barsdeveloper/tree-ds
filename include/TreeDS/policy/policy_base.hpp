#pragma once

#include <TreeDS/node/node_navigator.hpp>
#include <TreeDS/utility.hpp>

namespace md::detail {

template <
    typename ActualPolicy,
    typename NodePtr,
    typename NodeNavigator,
    typename Allocator>
class policy_base {

    template <typename, typename, typename, typename>
    friend class policy_base;

    protected:
    NodePtr current = nullptr;
    NodeNavigator navigator;
    Allocator allocator;

    public:
    /*
     * Create a policy that does not point at the beginning of the iteration but somewhere into the range. This means
     * that we must reconstruct the state like if we started at the beginning and arrived at current. Current can also
     * be nullptr, this means the policy pointes at the beginning of the sequence.
     */
    policy_base(NodePtr current, const Allocator& allocator) :
            current(current),
            allocator(allocator) {
    }

    policy_base(NodePtr current, const NodeNavigator& navigator, const Allocator& allocator) :
            current(current),
            navigator(navigator),
            allocator(allocator) {
    }

    template <
        typename OtherActualPolicy,
        typename OtherNode,
        typename OtherNodeNavigator,
        typename = std::enable_if_t<is_same_template<ActualPolicy, OtherActualPolicy>>,
        typename = std::enable_if_t<is_decay_pointed_same<OtherNode, NodePtr>>,
        typename = std::enable_if_t<std::is_convertible_v<OtherNodeNavigator, NodeNavigator>>>
    policy_base(const policy_base<OtherActualPolicy, OtherNode, OtherNodeNavigator, Allocator>& other) :
            policy_base(const_cast<NodePtr>(other.current), other.navigator, other.allocator) {
    }

    template <
        typename OtherActualPolicy,
        typename OtherNode,
        typename OtherNodeNavigator,
        typename = std::enable_if_t<is_same_template<ActualPolicy, OtherActualPolicy>>,
        typename = std::enable_if_t<is_decay_pointed_same<OtherNode, NodePtr>>,
        typename = std::enable_if_t<std::is_convertible_v<OtherNodeNavigator, NodeNavigator>>>
    policy_base(
        const policy_base<OtherActualPolicy, OtherNode, OtherNodeNavigator, Allocator>& other,
        const NodePtr current) :
            policy_base(const_cast<NodePtr>(current), other.navigator, other.allocator) {
    }

    public:
    NodePtr get_root() const {
        return this->root;
    }

    NodePtr get_current_node() const {
        return this->current;
    }

    NodeNavigator get_navigator() const {
        return this->navigator;
    }

    const Allocator& get_allocator() const {
        return this->allocator;
    }

    ActualPolicy& increment() {
        this->current = static_cast<ActualPolicy*>(this)->increment_impl();
        return *static_cast<ActualPolicy*>(this);
    }

    ActualPolicy& decrement() {
        this->current = static_cast<ActualPolicy*>(this)->decrement_impl();
        return *static_cast<ActualPolicy*>(this);
    }

    ActualPolicy& go_first() {
        this->current = static_cast<ActualPolicy*>(this)->go_first_impl();
        return *static_cast<ActualPolicy*>(this);
    }

    ActualPolicy& go_last() {
        this->current = static_cast<ActualPolicy*>(this)->go_last_impl();
        return *static_cast<ActualPolicy*>(this);
    }

    ActualPolicy& update(NodePtr current, NodePtr replacement) {
        // Subclasses can override this in roder to manage their status
        if (current == this->current) {
            this->current = replacement;
        }
        return *static_cast<ActualPolicy*>(this);
    }
};

/*
 * Tag classes are expected to provide two "get_instance" methods. A simple one that creates a policy which points at
 * at the beginning of a sequence and one that creates a policy that points somewhere in the range.
 */
template <template <typename...> class PolicyTemplate>
struct policy_tag {
    template <
        typename NodePtr,
        typename NodeNavigator,
        typename Allocator>
    PolicyTemplate<NodePtr, NodeNavigator, Allocator> get_instance(
        NodePtr current,
        const NodeNavigator& navigator,
        const Allocator& allocator) const {
        return PolicyTemplate<NodePtr, NodeNavigator, Allocator>(current, navigator, allocator);
    }
};

} // namespace md::detail
