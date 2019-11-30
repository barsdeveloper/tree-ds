#pragma once

#include <cassert> // assert
#include <iomanip> // std::setw()
#include <memory>  // std::unique_ptr
#include <ostream>
#include <tuple>       // std::make_tuple()
#include <type_traits> //std::enable_if_t
#include <utility>     // std::move(), std::forward()

#include <TreeDS/allocator_utility.hpp>
#include <TreeDS/node/node.hpp>
#include <TreeDS/node/struct_node.hpp>
#include <TreeDS/utility.hpp>

namespace md {

template <typename T>
class binary_node : public node<T, binary_node<T>> {

    /*   ---   FRIENDS   ---   */
    template <typename, typename, typename>
    friend class tree;

    template <typename, typename, typename, typename>
    friend class generative_navigator;

    template <typename A>
    friend void deallocate(A&, allocator_value_type<A>*);

    /*   ---   ATTRIBUTES   ---   */
    protected:
    binary_node* left  = nullptr;
    binary_node* right = nullptr;

    /*   ---   CONSTRUCTORS   ---   */
    public:
    using node<T, binary_node<T>>::node;

    // Move constructor
    binary_node(binary_node&& other) :
            node<T, binary_node<T>>(other.value),
            left(other.left),
            right(other.right) {
        if (this->left) {
            this->left->parent = this;
        }
        if (this->right) {
            this->right->parent = this;
        }
        other.parent = nullptr;
        other.left   = nullptr;
        other.right  = nullptr;
    }

    // Copy constructor using allocator
    template <typename Allocator = std::allocator<binary_node>>
    explicit binary_node(const binary_node& other, Allocator&& allocator = Allocator()) :
            node<T, binary_node<T>>(other.value),
            left(
                other.left
                    ? allocate(allocator, *other.left, allocator).release()
                    : nullptr),
            right(
                other.right
                    ? allocate(allocator, *other.right, allocator).release()
                    : nullptr) {
        this->attach_child();
    }

    // Converting constructor from struct_node using allocator
    template <
        typename ConvertibleT,
        typename FirstChild,
        typename NextSibling,
        typename Allocator = std::allocator<binary_node>,
        typename           = std::enable_if_t<std::is_convertible_v<ConvertibleT, T>>>
    explicit binary_node(
        const struct_node<ConvertibleT, FirstChild, NextSibling>& other,
        Allocator&& allocator = Allocator()) :
            node<T, binary_node>(other.get_value()),
            left(binary_node::allocate_left_child(other, allocator)),
            right(binary_node::allocate_right_child(other, allocator)) {
        static_assert(
            std::decay_t<decltype(other)>::children() <= 2,
            "A binary node must have at most 2 children.");
        this->attach_child();
    }

    // Emplacing copy constructor from struct_node using allocator
    template <
        typename... EmplaceArgs,
        typename FirstChild,
        typename NextSibling,
        typename Allocator = std::allocator<binary_node>,
        typename           = std::enable_if_t<std::is_constructible_v<T, EmplaceArgs...>>>
    explicit binary_node(
        const struct_node<std::tuple<EmplaceArgs...>, FirstChild, NextSibling>& other,
        Allocator&& allocator = Allocator()) :
            node<T, binary_node>(other.get_value()),
            left(binary_node::allocate_left_child(other, allocator)),
            right(binary_node::allocate_right_child(other, allocator)) {
        static_assert(
            std::decay_t<decltype(other)>::children() <= 2,
            "A binary node must have at most 2 children.");
        this->attach_child();
    }

    /*   ---   GETTERS   ---   */
    const binary_node* get_prev_sibling() const {
        return this->is_right_child()
            ? this->parent->left
            : nullptr;
    }

    binary_node* get_prev_sibling() {
        return this->is_right_child()
            ? this->parent->left
            : nullptr;
    }

    const binary_node* get_next_sibling() const {
        return this->is_left_child()
            ? this->parent->right
            : nullptr;
    }

    binary_node* get_next_sibling() {
        return this->is_left_child()
            ? this->parent->right
            : nullptr;
    }

    const binary_node* get_left_child() const {
        return this->left;
    }

    binary_node* get_left_child() {
        return this->left;
    }

    const binary_node* get_right_child() const {
        return this->right;
    }

    binary_node* get_right_child() {
        return this->right;
    }

    const binary_node* get_first_child() const {
        return this->left
            ? this->left
            : this->right;
    }

    binary_node* get_first_child() {
        return this->left
            ? this->left
            : this->right;
    }

    const binary_node* get_last_child() const {
        return this->right
            ? this->right
            : this->left;
    }

    binary_node* get_last_child() {
        return this->right
            ? this->right
            : this->left;
    }

    const binary_node* get_child(std::size_t index) const {
        switch (index) {
        case 0:
            return this->get_first_child();
        case 1:
            return this->left && this->right
                ? this->right
                : nullptr;
        default:
            return nullptr;
        }
    }

    binary_node* get_child(std::size_t index) {
        switch (index) {
        case 0:
            return this->get_first_child();
        case 1:
            return this->left && this->right
                ? this->right
                : nullptr;
        default:
            return nullptr;
        }
    }

    /*   ---   METHODS   ---   */
    private:
    template <typename U, typename FirstChild, typename NextSibling, typename Allocator>
    static binary_node* allocate_left_child(const struct_node<U, FirstChild, NextSibling>& node, Allocator& allocator) {
        if constexpr (!is_empty<FirstChild> && !is_empty_node<FirstChild>) {
            return allocate(allocator, node.get_first_child(), allocator).release();
        }
        return nullptr;
    }
    template <typename U, typename FirstChild, typename NextSibling, typename Allocator>
    static binary_node* allocate_right_child(const struct_node<U, FirstChild, NextSibling>& node, Allocator& allocator) {
        if constexpr (!is_empty<FirstChild>) {
            if constexpr (FirstChild::has_next_sibling()) {
                auto& sibling = node.get_first_child().get_next_sibling();
                if constexpr (!is_empty_node<std::decay_t<decltype(sibling)>>) {
                    return allocate(allocator, sibling, allocator).release();
                }
            }
        }
        return nullptr;
    }

    protected:
    /// Discard this whole subtree and replace it with node
    void replace_with(binary_node* node) {
        assert(node == nullptr || node->is_root());
        if (this->parent != nullptr) {
            if (this->is_left_child()) {
                this->parent->left = node;
            } else {
                this->parent->right = node;
            }
            if (node != nullptr) {
                this->parent->attach_child(node);
            }
            this->parent = nullptr;
        }
    }

    binary_node* prepend_child(binary_node* node) {
        if (node != nullptr) {
            if (this->right == nullptr) {
                this->right = this->left;
                this->left  = nullptr;
            }
            if (this->left == nullptr) {
                this->left = this->attach_child(node);
            }
        }
        return node;
    }

    binary_node* append_child(binary_node* node) {
        if (node != nullptr) {
            if (this->left == nullptr) {
                this->left  = this->right;
                this->right = nullptr;
            }
            if (this->right == nullptr) {
                return this->right = this->attach_child(node);
            }
        }
        return nullptr;
    }

    binary_node* do_assign_child_like(binary_node* child, const binary_node& reference_child) {
        assert(child);
        binary_node*& target_position = reference_child.is_left_child() ? this->left : this->right;
        assert(target_position == nullptr);
        target_position = child;
        return this->attach_child(target_position);
    }

    binary_node* attach_child(binary_node* node) {
        assert(node != nullptr);
        node->parent = this;
        return node;
    }

    void attach_child() {
        if (left != nullptr) {
            left->parent = this;
        }
        if (right != nullptr) {
            right->parent = this;
        }
    }

    public:
    bool is_first_child() const {
        return this->parent
            ? this->parent->get_first_child() == this
            : false;
    }

    bool is_last_child() const {
        return this->parent
            ? this->parent->get_last_child() == this
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

    std::size_t children() const {
        std::size_t result = 0u;
        if (this->left) {
            result += 1u;
        }
        if (this->right) {
            result += 1u;
        }
        return result;
    }

    std::size_t following_siblings() const {
        const binary_node* parent = this->parent;
        if (parent) {
            return parent->get_last_child() != this ? 1u : 0;
        }
        return 0u;
    }

    std::tuple<binary_node*, binary_node*> get_resources() {
        return std::make_tuple(this->left, this->right);
    }

    template <typename Allocator>
    binary_node* assign_child_like(unique_ptr_alloc<Allocator> child, const binary_node& reference_child) {
        return this->do_assign_child_like(child.release(), reference_child);
    }

    template <typename Allocator>
    unique_ptr_alloc<Allocator> allocate_assign_parent(Allocator& allocator, const binary_node& reference_copy) {
        assert(!reference_copy.is_root());
        unique_ptr_alloc<Allocator> parent = allocate(allocator, reference_copy.get_parent()->get_value());
        parent->do_assign_child_like(this, reference_copy);
        return std::move(parent);
    }

    /*   ---   COMPARISON   ---   */
    bool operator==(const binary_node& other) const {
        // Trivial case exclusion
        if (this->has_left_child() != other.has_left_child()
            || this->has_right_child() != other.has_right_child()) {
            return false;
        }
        // Test value for inequality
        if (this->value != other.value) {
            return false;
        }
        // Deep comparison (at this point both are either null or something)
        if (this->left && !this->left->operator==(*other.left)) {
            return false;
        }
        // Deep comparison (at this point both are either null or something)
        if (this->right && !this->right->operator==(*other.right)) {
            return false;
        }
        // All the possible false cases were tested, then it's true
        return true;
    }

    template <
        typename ConvertibleT = T,
        typename FirstChild,
        typename NextSibling,
        typename = std::enable_if_t<std::is_convertible_v<ConvertibleT, T>>>
    bool operator==(const struct_node<ConvertibleT, FirstChild, NextSibling>& other) const {
        // Too large tree
        if (other.subtree_arity() > 2) {
            return false;
        }
        // Test value for inequality
        if (!(this->value == other.get_value())) {
            return false;
        }
        // Trivial test left child presence
        if ((this->left == nullptr) != (is_empty<FirstChild> || is_empty_node<FirstChild>)
            || this->children() != other.children()) {
            return false;
        }
        if constexpr (!is_empty<FirstChild>) {
            // Test left child inequality
            if constexpr (!is_empty_node<FirstChild>) {
                if (!this->left->operator==(other.get_first_child())) {
                    return false;
                }
            }
            // Test right child inequality
            if constexpr (FirstChild::has_next_sibling()) {
                auto& sibling = other.get_first_child().get_next_sibling();
                if constexpr (!is_empty_node<std::decay_t<decltype(sibling)>>) {
                    if (!this->right->operator==(sibling)) {
                        return false;
                    }
                }
            }
        }
        // All the possible false cases were tested, then it's true
        return true;
    }

    long hash_code() const;
}; // namespace ds

// TODO C++20 replace with the ship operator (<=>)
// binary_node
template <typename T>
bool operator!=(const binary_node<T>& lhs, const binary_node<T>& rhs) {
    return !lhs.operator==(rhs);
}

// struct_node
template <
    typename T,
    typename ConvertibleT,
    typename FirstChild,
    typename NextSibling,
    typename = std::enable_if_t<std::is_convertible_v<ConvertibleT, T>>>
bool operator==(
    const struct_node<ConvertibleT, FirstChild, NextSibling>& lhs,
    const binary_node<T>& rhs) {
    return rhs.operator==(lhs);
}

template <
    typename T,
    typename ConvertibleT,
    typename FirstChild,
    typename NextSibling,
    typename = std::enable_if_t<std::is_convertible_v<ConvertibleT, T>>>
bool operator!=(
    const binary_node<T>& lhs,
    const struct_node<ConvertibleT, FirstChild, NextSibling>& rhs) {
    return !lhs.operator==(rhs);
}

template <
    typename T,
    typename ConvertibleT,
    typename FirstChild,
    typename NextSibling,
    typename = std::enable_if_t<std::is_convertible_v<ConvertibleT, T>>>
bool operator!=(
    const struct_node<ConvertibleT, FirstChild, NextSibling>& lhs,
    const binary_node<T>& rhs) {
    return !rhs.operator==(lhs);
}

} // namespace md
