#pragma once

#include <cstddef>    // std::nullptr_t
#include <functional> // std::mem_fn()
#include <tuple>
#include <utility> // std::move(), std::forward()

#include <TreeDS/allocator_utility.hpp>
#include <TreeDS/node/binary_node.hpp>
#include <TreeDS/node/node_base.hpp>
#include <TreeDS/node/struct_node.hpp>
#include <TreeDS/tree_base.hpp>
#include <TreeDS/utility.hpp>

namespace md::nary {

template <typename T>
class node : public md::node_base<T, node<T>> {

    template <typename, typename, bool>
    friend class md::tree_iterator;

    template <typename, typename, typename, typename>
    friend class md::tree_base;

    protected:
    node* first_child  = nullptr;
    node* next_sibling = nullptr;

    public:
    using md::node_base<T, node<T>>::node_base;

    public:
    /*   ---   Wide acceptance copy constructor using allocator   ---   */
    template <typename Allocator = std::allocator<node>>
    explicit node(
        const node<T>& other,
        Allocator&& allocator = std::allocator<node>()) :
            md::node_base<T, node<T>>(other.value),
            first_child(
                other.first_child != nullptr
                    ? attach(
                          allocate(
                              allocator,
                              *other.first_child,
                              allocator)
                              .release())
                    : nullptr),
            next_sibling(
                other.next_sibling != nullptr
                    ? allocate(
                          allocator,
                          *other.next_sibling,
                          allocator)
                          .release()
                    : nullptr) {
    }

    /*   ---   Move Constructor   ---   */
    explicit node(node&& other) :
            md::node_base<T, node>(std::move(other.get_value())),
            first_child(other.first_child),
            next_sibling(other.next_sibling) {
        other.move_resources_to(*this);
    }

    /*   ---   Conversion constructor from binary::node using allocator   ---   */
    template <typename Allocator = std::allocator<node>>
    explicit node(const md::binary::node<T>& other, Allocator&& allocator = std::allocator<node>()) :
            md::node_base<T, node>(other.get_value()),
            first_child(
                other.get_first_child() != nullptr
                    ? attach(
                          allocate(
                              allocator,
                              *other.get_first_child(),
                              allocator)
                              .release())
                    : nullptr),
            next_sibling(
                other.get_next_sibling() != nullptr
                    ? allocate(
                          allocator,
                          *other.get_next_sibling(),
                          allocator)
                          .release()
                    : nullptr) {
    }

    /*   ---   Construct from struct_node using allocator   ---   */
    template <
        typename ConvertibleT,
        typename... Nodes,
        typename Allocator = std::allocator<node>,
        CHECK_CONVERTIBLE(ConvertibleT, T)>
    explicit node(
        const struct_node<ConvertibleT, Nodes...>& other,
        Allocator&& allocator = std::allocator<node>()) :
            md::node_base<T, node>(other.get_value()),
            first_child(extract_children(other.get_children(), allocator)) {
    }

    /*   ---   Emplace from struct_node using allocator   ---   */
    template <
        typename... EmplaceArgs,
        typename... Nodes,
        typename Allocator = std::allocator<node>,
        CHECK_CONSTRUCTIBLE(T, EmplaceArgs...)>
    explicit node(
        const struct_node<std::tuple<EmplaceArgs...>, Nodes...>& other,
        Allocator&& allocator = std::allocator<node>()) :
            md::node_base<T, node>(other.get_value()),
            first_child(extract_children(other.get_children(), allocator)) {
    }

    private:
    template <typename Allocator, typename... Nodes>
    node* extract_children(const std::tuple<Nodes...>& nodes, Allocator& allocator) {
        // pointer to the considered child (which is itself a pointer to nary::node)
        node* result         = nullptr;
        node** current_child = &result;
        // lambda that assigns one child at time
        auto assign_child = [&](auto node) {
            *current_child           = node;                            // assign child
            (*current_child)->parent = this;                            // set the parent of that child
            current_child            = &(*current_child)->next_sibling; // go to next child
        };
        // lambda that constructs (by calling allocate) a nary::node from a struct_node
        auto allocate_child = [&](auto& structure_node) {
            static_assert(
                !std::is_same_v<decltype(structure_node.get_value()), std::nullptr_t>,
                "ds::node does not accept empty nodes");
            assign_child(allocate(allocator, structure_node, allocator).release());
        };
        std::apply(
            // call allocate_child for each element in the tuple other.get_children()
            [&](auto&... nodes) { (..., allocate_child(nodes)); },
            nodes);
        return result;
    }

    protected:
    /// Move the resources hold by this node to another node
    void move_resources_to(node& node) {
        if (this->first_child) {
            this->first_child->parent = &node;
        }
        if (this->next_sibling) {
            this->next_sibling->parent = &node;
        }
        this->parent       = nullptr;
        this->first_child  = nullptr;
        this->next_sibling = nullptr;
    }

    /// Discard this whole subtree and replace it with node
    void replace_with(node& node) {
        assert(node.is_root());
        assert(node.next_sibling == nullptr);
        if (this->parent != nullptr) {
            node.parent = this->parent;
            if (this->is_first_child()) {
                this->parent->first_child = &node;
            } else {
                this->get_prev_sibling()->next_sibling = &node;
            }
            if (this->next_sibling != nullptr) {
                node.next_sibling  = this->next_sibling;
                this->next_sibling = nullptr;
            }
            this->parent = nullptr;
        }
    }

    node* attach(node* n) {
        assert(n != nullptr);
        n->parent  = this;
        node* next = n->next_sibling;
        while (next) {
            this->attach(next);
            next = next->next_sibling;
        }
        return n;
    }

    public:
    bool is_last_child() const {
        return this->parent != nullptr && this->next_sibling == nullptr;
    }

    bool is_first_child() const {
        return this->parent
            ? this->parent->first_child == this
            : false;
    }

    const node* get_first_child() const {
        return this->first_child;
    }

    const node* get_last_child() const {
        return this->first_child
            ? keep_calling(*this->first_child, std::mem_fn(&node::get_next_sibling))
            : nullptr;
    }

    const node* get_prev_sibling() const {
        if (this->parent && this->parent->first_child) {
            const node* first = this->parent->first_child;
            if (first == this) {
                return nullptr;
            }
            return keep_calling(
                // starting from
                *first,
                // keep calling
                [](const node& node) {
                    return node.get_next_sibling();
                },
                // until
                [=](const node&, const node& next) {
                    return &next == this;
                },
                // then return
                [](const node& prev, const node&) {
                    return &prev;
                });
        } else {
            return nullptr;
        }
    }

    node* get_prev_sibling() {
        return const_cast<node*>(
            static_cast<const node*>(this)
                ->get_prev_sibling());
    }

    const node* get_next_sibling() const {
        return this->next_sibling;
    }

    const node* get_child(int index) const {
        const node* const* current = &this->first_child;
        for (int i = 0; i < index && *current != nullptr; ++i) {
            current = &(*current)->next_sibling;
        }
        return *current;
    }

    std::tuple<node*, node*> release() {
        return std::make_tuple(this->first_child, this->next_sibling);
    }

    bool operator==(const node& other) const {
        // Trivial case exclusion.
        if ((this->first_child == nullptr) != (other.first_child == nullptr)
            || (this->next_sibling == nullptr) != (other.next_sibling == nullptr)) {
            return false;
        }
        // Test value for inequality.
        if (this->value != other.value) {
            return false;
        }
        // Deep comparison (at this point both are either null or something).
        if (this->first_child && *this->first_child != *other.first_child) {
            return false;
        }
        // Deep comparison (at this point both are either null or something).
        if (this->next_sibling && *this->next_sibling != *other.next_sibling) {
            return false;
        }
        // All the possible false cases were tested, then it's true.
        return true;
    }

    bool operator==(const md::binary::node<T>& other) const {
        // Trivial case exclusion.
        if ((this->get_first_child() == nullptr) != (other.get_first_child() == nullptr)) {
            return false;
        }
        // Test value for inequality.
        if (this->get_value() != other.get_value()) {
            return false;
        }
        // Deep comparison (at this point both are either null or something).
        if (this->first_child && !this->first_child->operator==(*other.get_first_child())) {
            return false;
        }
        // Deep comparison (at this point both are either null or something).
        if (this->next_sibling && !this->next_sibling->operator==(*other.get_next_sibling())) {
            return false;
        }
        // All the possible false cases were tested, then it's true.
        return true;
    }

    template <
        typename ConvertibleT = T,
        typename... Nodes,
        CHECK_CONVERTIBLE(ConvertibleT, T)>
    bool operator==(const struct_node<ConvertibleT, Nodes...>& other) const {
        // One of the subtree has children while the other doesn't
        if ((this->first_child == nullptr) != (other.children_count() == 0)) {
            return false;
        }
        // Test value for inequality.
        if (this->value != other.get_value()) {
            return false;
        }
        // pointer to the considered child (which is itself a pointer to const nary::node)
        const node* const* current_child = &this->first_child;
        // lambda that constructs (by calling allocate) a nary::node from a struct_node
        auto compare_child = [&](auto& structure_node) -> bool {
            bool result;
            if constexpr (std::is_same_v<decltype(structure_node.get_value()), std::nullptr_t>) {
                result = *current_child == nullptr;
            } else { // not empty node
                result = *current_child && **current_child == structure_node;
            }
            current_child = &(*current_child)->next_sibling;
            return result;
        };
        return std::apply(
                   [&](auto&... nodes) {
                       // call compare_child for each element in the tuple other.get_children() and check all are true
                       return (compare_child(nodes) && ...);
                   },
                   other.get_children())
            && *current_child == nullptr;
    }

    template <typename MatcherNode>
    bool accept(const MatcherNode& matcher) {
        return matcher.match(*this);
    }
};

/*   ---   Expected equality properties  ---   */
template <typename T>
bool operator!=(const node<T>& lhs, const node<T>& rhs) {
    return !lhs.operator==(rhs);
}

template <
    typename T,
    typename ConvertibleT,
    typename... Children,
    CHECK_CONVERTIBLE(ConvertibleT, T)>
bool operator==(
    const struct_node<ConvertibleT, Children...>& lhs,
    const node<T>& rhs) {
    return rhs.operator==(lhs);
}

template <
    typename T,
    typename ConvertibleT,
    typename... Children,
    CHECK_CONVERTIBLE(ConvertibleT, T)>
bool operator!=(
    const node<T>& lhs,
    const struct_node<ConvertibleT, Children...>& rhs) {
    return !lhs.operator==(rhs);
}

template <
    typename T,
    typename ConvertibleT,
    typename... Children,
    CHECK_CONVERTIBLE(ConvertibleT, T)>
bool operator!=(
    const struct_node<ConvertibleT, Children...>& lhs,
    const node<T>& rhs) {
    return !rhs.operator==(lhs);
}

} // namespace md::nary
