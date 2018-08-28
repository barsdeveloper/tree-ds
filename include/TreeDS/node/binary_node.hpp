#pragma once

#include <TreeDS/node/node.hpp>
#include <TreeDS/node/struct_node.hpp>
#include <TreeDS/utility.hpp>
#include <cassert> // assert
#include <memory>  // std::unique_ptr
#include <type_traits>
#include <utility> // std::move(), std::forward()

namespace ds {

template <typename T>
class binary_node : public node<T, binary_node<T>> {

    template <typename, typename, bool>
    friend class tree_iterator;

    template <typename, typename, typename>
    friend class tree;

    protected:
    binary_node<T>* left  = nullptr;
    binary_node<T>* right = nullptr;

    public:
    using node<T, binary_node<T>>::node;

    /*   ---   Wide acceptance Copy Constructor   ---   */
    template <
        typename ConvertibleT = T,
        typename AllocateFn,
        CHECK_CONVERTIBLE(ConvertibleT, T)>
    explicit binary_node(const binary_node<ConvertibleT>& other, AllocateFn allocate) :
            node<T, binary_node<T>>(static_cast<T>(other.value)),
            left(
                other.left
                    ? attach(allocate(*other.left, allocate).release())
                    : nullptr),
            right(
                other.right
                    ? attach(allocate(*other.right, allocate).release())
                    : nullptr) {
    }

    // Construct from temporary_node using allocator
    template <
        typename ConvertibleT = T,
        typename... Nodes,
        typename AllocateFn,
        CHECK_CONVERTIBLE(ConvertibleT, T)>
    explicit binary_node(
        const struct_node<ConvertibleT, Nodes...>& other,
        AllocateFn allocate) :
            node<T, binary_node<T>>(other.get_value()) {
        static_assert(sizeof...(Nodes) <= 2, "A binary node must have at most 2 children");
        if constexpr (sizeof...(Nodes) >= 1) {
            const auto& left = get_child<0>(other);
            if constexpr (!std::is_same_v<decltype(left.get_value()), std::nullptr_t>) {
                this->left = attach(allocate(left, allocate).release());
            }
        }
        if constexpr (sizeof...(Nodes) >= 2) {
            const auto& right = get_child<1>(other);
            if constexpr (!std::is_same_v<decltype(right.get_value()), std::nullptr_t>) {
                this->right = attach(allocate(right, allocate).release());
            }
        }
    }

    // Move constructor
    binary_node(binary_node&& other) :
            node<T, binary_node<T>>(other.value),
            left(other.left),
            right(other.right) {
        other.move_resources_to(*this);
    }

    ~binary_node() = default;

    protected:
    binary_node* attach(binary_node* node) {
        assert(node != nullptr);
        node->parent = this;
        return node;
    }

    void move_resources_to(binary_node& node) {
        if (this->parent) {
            if (this->parent->left == this) {
                this->parent->left = &node;
            }
            if (this->parent->right == this) {
                this->parent->right = &node;
            }
        }
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

    public:
    const binary_node* get_left() const {
        return this->left;
    }

    const binary_node* get_right() const {
        return this->right;
    }

    const binary_node* first_child() const {
        return this->left
            ? this->left
            : this->right;
    }

    const binary_node* last_child() const {
        return this->right
            ? this->right
            : this->left;
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

    long hash_code() const;

    template <
        typename ConvertibleT = T,
        CHECK_CONVERTIBLE(ConvertibleT, T)>
    bool operator==(const binary_node<ConvertibleT>& other) const {
        // Trivial case exclusion.
        if ((this->left == nullptr) != (other.left == nullptr)
            || (this->right == nullptr) != (other.right == nullptr)) {
            return false;
        }
        // Test value for inequality.
        if (this->value != other.value) {
            return false;
        }
        // Deep comparison (at this point both are either null or something).
        if (this->left && *this->left != *other.left) {
            return false;
        }
        // Deep comparison (at this point both are either null or something).
        if (this->right && *this->right != *other.right) {
            return false;
        }
        // All the possible false cases were tested, then it's true.
        return true;
    }

    template <
        typename ConvertibleT = T,
        CHECK_CONVERTIBLE(ConvertibleT, T)>
    bool operator!=(const binary_node<ConvertibleT>& other) const {
        return !(*this == other);
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
        // All the possible false cases were tested, then it's true.
        return true;
    }
};

} // namespace ds
