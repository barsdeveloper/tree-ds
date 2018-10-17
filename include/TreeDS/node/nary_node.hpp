#pragma once

#include <cstddef>    // std::nullptr_t
#include <functional> // std::mem_fn()
#include <tuple>
#include <utility> // std::move(), std::forward()

#include <TreeDS/allocator_utility.hpp>
#include <TreeDS/node/binary_node.hpp>
#include <TreeDS/node/node.hpp>
#include <TreeDS/node/struct_node.hpp>
#include <TreeDS/utility.hpp>

namespace ds {

template <typename T>
class nary_node : public node<T, nary_node<T>> {

    template <typename, typename, bool>
    friend class tree_iterator;

    template <typename, typename, typename, typename>
    friend class tree;

    protected:
    nary_node* first_child  = nullptr;
    nary_node* next_sibling = nullptr;

    public:
    using node<T, nary_node<T>>::node;

    public:
    /*   ---   Wide acceptance copy constructor using allocator   ---   */
    template <typename Allocator = std::allocator<nary_node>>
    explicit nary_node(
        const nary_node<T>& other,
        Allocator&& allocator = std::allocator<nary_node>()) :
            node<T, nary_node<T>>(other.value),
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
    explicit nary_node(nary_node&& other) :
            node<T, nary_node>(std::move(other.get_value())),
            first_child(other.first_child),
            next_sibling(other.next_sibling) {
        other.move_resources_to(*this);
    }

    /*   ---   Conversion constructor from binary_node using allocator   ---   */
    template <typename Allocator = std::allocator<nary_node>>
    explicit nary_node(const binary_node<T>& other, Allocator&& allocator = std::allocator<nary_node>()) :
            node<T, nary_node>(other.get_value()),
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
        typename Allocator = std::allocator<nary_node>,
        CHECK_CONVERTIBLE(ConvertibleT, T)>
    explicit nary_node(
        const struct_node<ConvertibleT, Nodes...>& other,
        Allocator&& allocator = std::allocator<nary_node>()) :
            node<T, nary_node>(other.get_value()),
            first_child(extract_children(other.get_children(), allocator)) {
    }

    /*   ---   Emplace from struct_node using allocator   ---   */
    template <
        typename... EmplaceArgs,
        typename... Nodes,
        typename Allocator = std::allocator<nary_node>,
        CHECK_CONSTRUCTIBLE(T, EmplaceArgs...)>
    explicit nary_node(
        const struct_node<std::tuple<EmplaceArgs...>, Nodes...>& other,
        Allocator&& allocator = std::allocator<nary_node>()) :
            node<T, nary_node>(other.get_value()),
            first_child(extract_children(other.get_children(), allocator)) {
    }

    private:
    template <typename... Nodes, typename Allocator>
    nary_node* extract_children(const std::tuple<Nodes...>& nodes, Allocator&& allocator) {
        // pointer to the considered child (which is itself a pointer to nary_node)
        nary_node* result         = nullptr;
        nary_node** current_child = &result;
        // lambda that assigns one child at time
        auto assign_child = [&](auto node) {
            *current_child           = node;                            // assign child
            (*current_child)->parent = this;                            // set the parent of that child
            current_child            = &(*current_child)->next_sibling; // go to next child
        };
        // lambda that constructs (by calling allocate) a nary_node from a struct_node
        auto allocate_child = [&](auto& structure_node) {
            static_assert(
                !std::is_same_v<decltype(structure_node.get_value()), std::nullptr_t>,
                "ds::nary_node does not accept empty nodes");
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
    void move_resources_to(nary_node& node) {
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
    void replace_with(nary_node& node) {
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

    nary_node* attach(nary_node* node) {
        assert(node != nullptr);
        node->parent    = this;
        nary_node* next = node->next_sibling;
        while (next) {
            this->attach(next);
            next = next->next_sibling;
        }
        return node;
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

    const nary_node* get_first_child() const {
        return this->first_child;
    }

    const nary_node* get_last_child() const {
        return this->first_child
            ? keep_calling(*this->first_child, std::mem_fn(&nary_node::get_next_sibling))
            : nullptr;
    }

    const nary_node* get_prev_sibling() const {
        if (this->parent && this->parent->first_child) {
            const nary_node* first = this->parent->first_child;
            if (first == this) {
                return nullptr;
            }
            return keep_calling(
                // starting from
                *first,
                // keep calling
                [](const nary_node& node) {
                    return node.get_next_sibling();
                },
                // until
                [=](const nary_node&, const nary_node& next) {
                    return &next == this;
                },
                // then return
                [](const nary_node& prev, const nary_node&) {
                    return &prev;
                });
        } else {
            return nullptr;
        }
    }

    nary_node* get_prev_sibling() {
        return const_cast<nary_node*>(
            static_cast<const nary_node*>(this)
                ->get_prev_sibling());
    }

    const nary_node* get_next_sibling() const {
        return this->next_sibling;
    }

    const nary_node* get_child(int index) const {
        const nary_node* const* current = &this->first_child;
        for (int i = 0; i < index && *current != nullptr; ++i) {
            current = &(*current)->next_sibling;
        }
        return *current;
    }

    std::tuple<nary_node*, nary_node*> release() {
        return std::make_tuple(this->first_child, this->next_sibling);
    }

    bool operator==(const nary_node& other) const {
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

    bool operator==(const binary_node<T>& other) const {
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
        // pointer to the considered child (which is itself a pointer to const nary_node)
        const nary_node* const* current_child = &this->first_child;
        // lambda that constructs (by calling allocate) a nary_node from a struct_node
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
bool operator!=(const nary_node<T>& lhs, const nary_node<T>& rhs) {
    return !lhs.operator==(rhs);
}

template <
    typename T,
    typename ConvertibleT,
    typename... Children,
    CHECK_CONVERTIBLE(ConvertibleT, T)>
bool operator==(
    const struct_node<ConvertibleT, Children...>& lhs,
    const nary_node<T>& rhs) {
    return rhs.operator==(lhs);
}

template <
    typename T,
    typename ConvertibleT,
    typename... Children,
    CHECK_CONVERTIBLE(ConvertibleT, T)>
bool operator!=(
    const nary_node<T>& lhs,
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
    const nary_node<T>& rhs) {
    return !rhs.operator==(lhs);
}

} // namespace ds
