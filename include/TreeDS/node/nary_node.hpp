#pragma once

#include <cstddef>    // std::nullptr_t
#include <functional> // std::mem_fn()
#include <tuple>
#include <utility> // std::move(), std::forward()

#include <TreeDS/allocator_utility.hpp>
#include <TreeDS/node/node.hpp>
#include <TreeDS/node/struct_node.hpp>
#include <TreeDS/utility.hpp>

namespace ds {

template <typename T>
class nary_node : public node<T, nary_node<T>> {

    template <typename, template <typename> class, bool>
    friend class tree_iterator;

    template <typename, template <typename> class, template <typename> class, typename>
    friend class tree;

    protected:
    nary_node* first_child  = nullptr;
    nary_node* next_sibling = nullptr;

    public:
    using node<T, nary_node<T>>::node;

    public:
    /*   ---   Wide acceptance copy constructor using allocator   ---   */
    template <
        typename ConvertibleT = T,
        typename Allocator    = std::allocator<nary_node>,
        CHECK_CONVERTIBLE(ConvertibleT, T)>
    explicit nary_node(
        const nary_node<ConvertibleT>& other,
        Allocator&& allocator = std::allocator<nary_node>()) :
            node<T, nary_node<T>>(static_cast<T>(other.value)),
            first_child(
                other.first_child
                    ? this->attach(allocate(allocator, *other.first_child, allocator).release())
                    : nullptr),
            next_sibling(
                other.next_sibling
                    ? this->attach(allocate(allocator, *other.next_sibling, allocator).release())
                    : nullptr) {
    }

    /*   ---   Move Constructor   ---   */
    explicit nary_node(nary_node&& other) :
            node<T, nary_node>(std::move(other.get_value())),
            first_child(other.first_child),
            next_sibling(other.next_sibling) {
        other.move_resources_to(*this);
    }

    /*   ---   Construct from struct_node using allocator   ---   */
    template <
        typename ConvertibleT = T,
        typename... Nodes,
        typename Allocator = std::allocator<nary_node>,
        CHECK_CONVERTIBLE(ConvertibleT, T)>
    explicit nary_node(
        const struct_node<ConvertibleT, Nodes...>& other,
        Allocator&& allocator = std::allocator<nary_node>()) :
            node<T, nary_node>(other.get_value()) {
        // pointer to the considered child (which is itself a pointer to nary_node)
        nary_node** current_child = &this->first_child;
        // lambda that assigns one child at time
        auto assign_child = [&](auto&& node) {
            *current_child           = node.release();                  // assign child
            (*current_child)->parent = this;                            // set the parent of that child
            current_child            = &(*current_child)->next_sibling; // go to next child
        };
        // lambda that constructs (by calling allocate) a nary_node from a struct_node
        auto allocate_child = [&](auto& structure_node) {
            static_assert(
                !std::is_same_v<decltype(structure_node.get_value()), std::nullptr_t>,
                "ds::nary_node does not accept struct_node with empty nodes");
            assign_child(allocate(allocator, structure_node, allocator));
        };
        std::apply(
            [&](auto&... nodes) {
                // call allocate_child for each element in the tuple other.get_children()
                (allocate_child(nodes), ...);
            },
            other.get_children());
    }

    protected:
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

    public:
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

    auto release() {
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
