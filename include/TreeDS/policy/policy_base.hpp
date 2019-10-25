#pragma once

#include <memory> // std::allocator_traits

#include <TreeDS/utility.hpp>

namespace md::detail {

template <
    typename ActualPolicy,
    typename NodePtr,
    typename Navigator,
    typename Allocator>
class policy_base {

    template <typename, typename, typename, typename>
    friend class policy_base;

    public:
    using allocator_type = typename std::allocator_traits<Allocator>::template rebind_alloc<NodePtr>;

    static_assert(
        std::is_same_v<typename Navigator::node_pointer, NodePtr>,
        "Navigator must use the same NodePtr template parameter as NodePtr");
    static_assert(
        std::is_same_v<typename std::allocator_traits<Allocator>::value_type, NodePtr>,
        "Allocator type must have a value_type = NodePtr");

    protected:
    NodePtr current = nullptr;
    Navigator navigator {};
    allocator_type allocator {};

    public:
    policy_base() {
    }

    /*
     * Create a policy that does not point at the beginning of the iteration but somewhere into the range. This means
     * that we must reconstruct the state like if we started at the beginning and arrived at current. Current can also
     * be nullptr, this means the policy pointes at the beginning of the sequence.
     */
    policy_base(NodePtr current, const Allocator& allocator) :
            current(current),
            allocator(allocator) {
        this->cast()->fix_navigator_root();
    }

    policy_base(NodePtr current, const Navigator& navigator, const Allocator& allocator) :
            current(current),
            navigator(navigator),
            allocator(allocator) {
        this->cast()->fix_navigator_root();
    }

    template <
        typename OtherActualPolicy,
        typename OtherNodePtr,
        typename OtherNavigator,
        typename OtherAllocator,
        typename = std::enable_if_t<is_same_template<OtherActualPolicy, ActualPolicy>>,
        typename = std::enable_if_t<std::is_convertible_v<OtherNodePtr, NodePtr>>,
        typename = std::enable_if_t<std::is_convertible_v<OtherNavigator, Navigator>>>
    policy_base(const policy_base<OtherActualPolicy, OtherNodePtr, OtherNavigator, OtherAllocator>& other) :
            policy_base(other.current, other.navigator, other.allocator) {
    }

    template <
        typename OtherActualPolicy,
        typename OtherNodePtr,
        typename OtherNavigator,
        typename OtherAllocator,
        typename = std::enable_if_t<is_same_template<ActualPolicy, OtherActualPolicy>>,
        typename = std::enable_if_t<std::is_convertible_v<OtherNodePtr, NodePtr>>,
        typename = std::enable_if_t<std::is_convertible_v<OtherNavigator, Navigator>>>
    policy_base(
        const policy_base<OtherActualPolicy, OtherNodePtr, OtherNavigator, OtherAllocator>& other,
        const NodePtr current) :
            policy_base(current, other.navigator, other.allocator) {
    }

    private:
    constexpr void fix_navigator_root() {
        if (!this->navigator.get_root()) {
            this->navigator.set_root(this->current);
        }
    }

    protected:
    const ActualPolicy* cast() const {
        return static_cast<ActualPolicy*>(this);
    }

    ActualPolicy* cast() {
        return static_cast<ActualPolicy*>(this);
    }

    public:
    NodePtr get_root() const {
        return this->root;
    }

    NodePtr get_current_node() const {
        return this->current;
    }

    Navigator get_navigator() const {
        return this->navigator;
    }

    const allocator_type& get_allocator() const {
        return this->allocator;
    }

    ActualPolicy& increment() {
        assert(this->current);
        this->current = this->cast()->increment_impl();
        return *this->cast();
    }

    ActualPolicy& decrement() {
        assert(this->current);
        this->current = this->cast()->decrement_impl();
        return *this->cast();
    }

    ActualPolicy& go_first() {
        assert(this->navigator.get_root());
        this->current = this->cast()->go_first_impl();
        return *this->cast();
    }

    ActualPolicy& go_last() {
        assert(this->navigator.get_root());
        this->current = this->cast()->go_last_impl();
        return *this->cast();
    }

    ActualPolicy& update(NodePtr current, NodePtr replacement) {
        // Subclasses can override this in roder to manage their status
        if (current == this->current) {
            this->current = replacement;
        }
        return *this->cast();
    }

    operator bool() const {
        return static_cast<bool>(this->current);
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
    PolicyTemplate<
        NodePtr,
        NodeNavigator,
        typename std::allocator_traits<Allocator>::template rebind_alloc<NodePtr>>
    get_instance(
        NodePtr current,
        const NodeNavigator& navigator,
        const Allocator& allocator) const {
        using allocator_t = typename std::allocator_traits<Allocator>::template rebind_alloc<NodePtr>;
        return {current, navigator, static_cast<allocator_t>(allocator)};
    }
};

} // namespace md::detail
