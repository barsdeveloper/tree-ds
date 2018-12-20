#pragma once

#include <cassert> // assert
#include <memory>  // std::unique_ptr
#include <tuple>   // std::make_tuple()
#include <type_traits>
#include <utility> // std::move(), std::forward()
#include <variant>

#include <TreeDS/allocator_utility.hpp>
#include <TreeDS/node/node_base.hpp>
#include <TreeDS/node/struct_node.hpp>
#include <TreeDS/tree_base.hpp>
#include <TreeDS/utility.hpp>

namespace md {

template <typename, typename, bool>
class tree_iterator;

template <typename, typename, typename, typename>
class tree;

} // namespace md

namespace md::binary {

template <typename T>
class node : public md::node_base<T, node<T>> {

    template <typename, typename, bool>
    friend class md::tree_iterator;

    template <typename, typename, typename, typename>
    friend class md::tree_base;

    template <typename A>
    friend void md::deallocate(A&, typename A::value_type*);

    public:
    using super = md::node_base<T, node>;

    protected:
    node* left  = nullptr;
    node* right = nullptr;

    public:
    using md::node_base<T, node<T>>::node_base;

    /*   ---   Wide acceptance copy constructor using allocator   ---   */
    template <typename Allocator = std::allocator<node>>
    explicit node(
        const node<T>& other,
        Allocator&& allocator = std::allocator<node>()) :
            md::node_base<T, node<T>>(other.value),
            left(
                other.left
                    ? attach_children(allocate(allocator, *other.left, allocator).release())
                    : nullptr),
            right(
                other.right
                    ? attach_children(allocate(allocator, *other.right, allocator).release())
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
            left(extract_left(other.get_children(), std::forward<Allocator>(allocator))),
            right(extract_right(other.get_children(), std::forward<Allocator>(allocator))) {
        static_assert(sizeof...(Nodes) <= 2, "A binary node must have at most 2 children.");
        attach_children();
    }

    /*   ---   Emplacing from struct_node using allocator   ---   */
    template <
        typename... EmplaceArgs,
        typename... Nodes,
        typename Allocator = std::allocator<node>,
        CHECK_CONSTRUCTIBLE(T, EmplaceArgs...)>
    explicit node(
        const struct_node<std::tuple<EmplaceArgs...>, Nodes...>& other,
        Allocator&& allocator = std::allocator<node>()) :
            md::node_base<T, node>(other.get_value()),
            left(extract_left(other.get_children(), std::forward<Allocator>(allocator))),
            right(extract_right(other.get_children(), std::forward<Allocator>(allocator))) {
        static_assert(sizeof...(Nodes) <= 2, "A binary node must have at most 2 children.");
        attach_children();
    }

    /*   ---   Move constructor   ---   */
    node(node&& other) :
            md::node_base<T, node<T>>(other.value),
            left(other.left),
            right(other.right) {
        other.move_resources_to(*this);
    }

    ~node() = default;

    private:
    template <typename... Nodes, typename Allocator>
    node* extract_left(const std::tuple<Nodes...>& children, Allocator&& allocator) {
        if constexpr (sizeof...(Nodes) >= 1) {
            const auto& left = std::get<0>(children);
            if constexpr (!std::is_same_v<decltype(left.get_value()), std::nullptr_t>) {
                return allocate(allocator, left, std::forward<Allocator>(allocator)).release();
            }
        }
        return nullptr;
    }

    template <typename... Nodes, typename Allocator>
    node* extract_right(const std::tuple<Nodes...>& children, Allocator&& allocator) {
        if constexpr (sizeof...(Nodes) >= 2) {
            const auto& right = std::get<1>(children);
            if constexpr (!std::is_same_v<decltype(right.get_value()), std::nullptr_t>) {
                return allocate(allocator, right, std::forward<Allocator>(allocator)).release();
            }
        }
        return nullptr;
    }

    protected:
    /// Move the resources hold by this node to another node
    void move_resources_to(node& node) {
        if (this->left) {
            this->left->parent = &node;
        }
        if (this->right) {
            this->right->parent = &node;
        }
        this->parent = nullptr;
        this->left   = nullptr;
        this->right  = nullptr;
    }

    /// Discard this whole subtree and replace it with node
    void replace_with(node& node) {
        assert(node.is_root());
        if (this->parent != nullptr) {
            if (this->is_left_child()) {
                this->parent->left = &node;
            } else {
                this->parent->right = &node;
            }
            this->parent->attach_children(&node);
            this->parent = nullptr;
        }
    }

    node* attach_children(node* node) {
        assert(node != nullptr);
        node->parent = this;
        return node;
    }

    void attach_children() {
        if (left != nullptr) {
            left->parent = this;
        }
        if (right != nullptr) {
            right->parent = this;
        }
    }

    public:
    bool is_last_child() const {
        return this->parent
            ? this->parent->get_last_child() == this
            : false;
    }

    bool is_first_child() const {
        return this->parent
            ? this->parent->get_first_child() == this
            : false;
    }

    bool is_left_child() const {
        return this->parent
            ? this == this->parent->left
            : false;
    }

    bool is_right_child() const {
        return this->parent
            ? this == this->parent->right
            : false;
    }

    bool has_left_child() const {
        return this->left != nullptr;
    }

    bool has_right_child() const {
        return this->right != nullptr;
    }

    const node* get_left_child() const {
        return this->left;
    }

    const node* get_right_child() const {
        return this->right;
    }

    const node* get_first_child() const {
        return this->left
            ? this->left
            : this->right;
    }

    const node* get_last_child() const {
        return this->right
            ? this->right
            : this->left;
    }

    const node* get_next_sibling() const {
        auto parent = this->parent;
        if (parent) {
            if (this == parent->left) return parent->right;
        }
        return nullptr;
    }

    const node* get_prev_sibling() const {
        auto parent = this->parent;
        if (parent) {
            if (this == parent->right) return parent->left;
        }
        return nullptr;
    }

    std::tuple<node*, node*> release() {
        return std::make_tuple(this->left, this->right);
    }

    long hash_code() const;

    bool operator==(const node& other) const {
        // Trivial case exclusion.
        if (this->has_left_child() != other.has_left_child()
            || this->has_right_child() != other.has_right_child()) {
            return false;
        }
        // Test value for inequality.
        if (this->value != other.value) {
            return false;
        }
        // Deep comparison (at this point both are either null or something).
        if (this->left && !this->left->operator==(*other.left)) {
            return false;
        }
        // Deep comparison (at this point both are either null or something).
        if (this->right && !this->right->operator==(*other.right)) {
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
        // Too large tree
        if (other.children_count() > 2) {
            return false;
        }
        // Test value for inequality.
        if (!(this->value == other.get_value())) {
            return false;
        }
        if constexpr (sizeof...(Nodes) >= 1) {
            const auto& left = get_child<0>(other);
            if constexpr (!std::is_same_v<decltype(left.get_value()), std::nullptr_t>) {
                static_assert(
                    std::is_convertible_v<std::decay_t<decltype(left.get_value())>, T>,
                    "The struct_node passed has a LEFT child with a value that is not compatible with T.");
                if (!this->left || *this->left != left) {
                    return false;
                }
            } else if (this->left) {
                return false;
            }
        } else if (this->left) {
            return false;
        }
        if constexpr (sizeof...(Nodes) >= 2) {
            const auto& right = get_child<1>(other);
            if constexpr (!std::is_same_v<decltype(right.get_value()), std::nullptr_t>) {
                static_assert(
                    std::is_convertible_v<std::decay_t<decltype(right.get_value())>, T>,
                    "The struct_node passed has a RIGHT child with a value that is not compatible with T.");
                if (!this->right || *this->right != right) {
                    return false;
                }
            } else if (this->right) {
                return false;
            }
        } else if (this->right) {
            return false;
        }
        // All the possible false cases were tested, then it's true.
        return true;
    }

}; // namespace ds

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

} // namespace md::binary
