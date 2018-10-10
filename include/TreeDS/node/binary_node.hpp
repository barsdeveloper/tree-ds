#pragma once

#include <cassert> // assert
#include <memory>  // std::unique_ptr
#include <tuple>   // std::make_tuple()
#include <type_traits>
#include <utility> // std::move(), std::forward()
#include <variant>

#include <TreeDS/allocator_utility.hpp>
#include <TreeDS/node/node.hpp>
#include <TreeDS/node/struct_node.hpp>
#include <TreeDS/utility.hpp>

namespace ds {

template <typename T>
class binary_node : public node<T, binary_node<T>> {

    template <typename, typename, bool>
    friend class tree_iterator;

    template <typename, typename, typename, typename>
    friend class tree;

    template <typename A>
    friend void deallocate(A&, typename A::value_type*);

    public:
    using super = node<T, binary_node>;

    protected:
    binary_node* left  = nullptr;
    binary_node* right = nullptr;

    public:
    using node<T, binary_node<T>>::node;

    /*   ---   Wide acceptance copy constructor using allocator   ---   */
    template <
        typename ConvertibleT = T,
        typename Allocator    = std::allocator<binary_node>,
        CHECK_CONVERTIBLE(ConvertibleT, T)>
    explicit binary_node(
        const binary_node<ConvertibleT>& other,
        Allocator&& allocator = std::allocator<binary_node>()) :
            node<T, binary_node<T>>(static_cast<T>(other.value)),
            left(
                other.left
                    ? this->attach(allocate(allocator, *other.left, allocator).release())
                    : nullptr),
            right(
                other.right
                    ? this->attach(allocate(allocator, *other.right, allocator).release())
                    : nullptr) {
    }

    /*   ---   Construct from struct_node using allocator   ---   */
    template <
        typename ConvertibleT = T,
        typename... Nodes,
        typename Allocator = std::allocator<binary_node>,
        CHECK_CONVERTIBLE(ConvertibleT, T)>
    explicit binary_node(
        const struct_node<ConvertibleT, Nodes...>& other,
        Allocator&& allocator = std::allocator<binary_node>()) :
            node<T, binary_node>(other.get_value()) {
        static_assert(sizeof...(Nodes) <= 2, "A binary node must have at most 2 children.");

        if constexpr (sizeof...(Nodes) >= 1) {
            const auto& left = get_child<0>(other);
            if constexpr (!std::is_same_v<decltype(left.get_value()), std::nullptr_t>) {
                static_assert(
                    std::is_convertible_v<std::decay_t<decltype(left.get_value())>, T>,
                    "The struct_node passed has a LEFT child with a value that is not compatible with T.");
                this->left = this->attach(allocate(allocator, left, allocator).release());
            }
        }
        if constexpr (sizeof...(Nodes) >= 2) {
            const auto& right = get_child<1>(other);
            if constexpr (!std::is_same_v<decltype(right.get_value()), std::nullptr_t>) {
                static_assert(
                    std::is_convertible_v<std::decay_t<decltype(right.get_value())>, T>,
                    "The struct_node passed has a RIGHT child with a value that is not compatible with T.");
                this->right = this->attach(allocate(allocator, right, allocator).release());
            }
        }
    }

    /*   ---   Move constructor   ---   */
    binary_node(binary_node&& other) :
            node<T, binary_node<T>>(other.value),
            left(other.left),
            right(other.right) {
        other.move_resources_to(*this);
    }

    ~binary_node() = default;

    protected:
    /// Move the resources hold by this node to another node
    void move_resources_to(binary_node& node) {
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
    void replace_with(binary_node& node) {
        assert(node.is_root());
        if (this->parent != nullptr) {
            if (this->is_left_child()) {
                this->parent->left = &node;
            } else {
                this->parent->right = &node;
            }
            this->parent->attach(&node);
            this->parent = nullptr;
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

    const binary_node* get_left_child() const {
        return this->left;
    }

    const binary_node* get_right_child() const {
        return this->right;
    }

    const binary_node* get_first_child() const {
        return this->left
            ? this->left
            : this->right;
    }

    const binary_node* get_last_child() const {
        return this->right
            ? this->right
            : this->left;
    }

    const binary_node* get_next_sibling() const {
        auto parent = this->parent;
        if (parent) {
            if (this == parent->left) return parent->right;
        }
        return nullptr;
    }

    const binary_node* get_prev_sibling() const {
        auto parent = this->parent;
        if (parent) {
            if (this == parent->right) return parent->left;
        }
        return nullptr;
    }

    std::tuple<binary_node*, binary_node*> release() {
        return std::make_tuple(this->left, this->right);
    }

    long hash_code() const;

    bool operator==(const binary_node& other) const {
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
bool operator!=(const binary_node<T>& lhs, const binary_node<T>& rhs) {
    return !lhs.operator==(rhs);
}

template <
    typename T,
    typename ConvertibleT,
    typename... Children,
    CHECK_CONVERTIBLE(ConvertibleT, T)>
bool operator==(
    const struct_node<ConvertibleT, Children...>& lhs,
    const binary_node<T>& rhs) {
    return rhs.operator==(lhs);
}

template <
    typename T,
    typename ConvertibleT,
    typename... Children,
    CHECK_CONVERTIBLE(ConvertibleT, T)>
bool operator!=(
    const binary_node<T>& lhs,
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
    const binary_node<T>& rhs) {
    return !rhs.operator==(lhs);
}

} // namespace ds
