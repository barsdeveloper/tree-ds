#pragma once

#include <TreeDS/node.hpp>
#include <TreeDS/temp_node.hpp>
#include <TreeDS/utility.hpp>
#include <cassert> // assert
#include <memory>  // std::unique_ptr
#include <type_traits>
#include <utility> // std::move(), std::forward()
#include <variant>

namespace ds {

template <typename T>
class binary_node : public node<T, binary_node<T>> {

    using node_t = node<T, binary_node<T>>;

    template <typename, typename, bool>
    friend class tree_iterator;

    template <typename, typename, typename>
    friend class tree;

    protected:
    binary_node<T>* _left = nullptr;
    binary_node<T>* _right = nullptr;

    public:
    using node_t::node;

    template <
        typename ConvertibleT = T,
        typename AllocateFn,
        CHECK_CONVERTIBLE(ConvertibleT, T)>
    explicit binary_node(const binary_node<ConvertibleT>& other, AllocateFn allocate) :
            node_t(static_cast<T>(other._value)),
            _left(other._left ? attach(allocate(*other._left, allocate).release()) : nullptr),
            _right(other._right ? attach(allocate(*other._right, allocate).release()) : nullptr) {
    }

    // Construct from temporary_node using allocator
    template <
        typename ConvertibleT = T,
        typename AllocateFn,
        CHECK_CONVERTIBLE(ConvertibleT, T)>
    explicit binary_node(const temp_node<ConvertibleT>& other, AllocateFn allocate) :
            node_t(std::move(other.get_value())),
            _left(nullptr),
            _right(nullptr) {
        if (!other.has_children()) {
            return;
        }
        assert(other.children_number() <= 2);
        auto it = other.begin();
        {
           this->_left = std::holds_alternative<temp_node<ConvertibleT>>(*it)
                ? attach(
                      allocate(
                          std::get<temp_node<ConvertibleT>>(*it),
                          allocate)
                          .release())
                : nullptr;
            ++it;
        }
        if (it != other.end()) {
           this->_right = std::holds_alternative<temp_node<ConvertibleT>>(*it)
                ? attach(
                      allocate(
                          std::get<temp_node<ConvertibleT>>(*it),
                          allocate)
                          .release())
                : nullptr;
            ++it;
        }
        assert(it == other.end());
    }

    // Move constructor
    binary_node(binary_node&& other) :
            node_t(other._value),
            _left(other._left),
            _right(other._right) {
        other.move_resources_to(*this);
    }

    ~binary_node() = default;

    protected:
    binary_node* attach(binary_node* node) {
        assert(node != nullptr);
        node->_parent = this;
        return node;
    }

    void move_resources_to(binary_node& node) {
        if (this->_parent) {
            if (this->_parent->_left == this) {
                this->_parent->_left = &node;
            }
            if (this->_parent->_right == this) {
                this->_parent->_right = &node;
            }
        }
        if (this->_left) {
            this->_left->_parent = &node;
        }
        if (this->_right) {
            this->_right->_parent = &node;
        }
        this->_parent = nullptr;
        this->_left   = nullptr;
        this->_right  = nullptr;
    }

    public:
    const binary_node* left_child() const {
        return this->_left;
    }

    const binary_node* right_child() const {
        return this->_right;
    }

    const binary_node* first_child() const {
        return this->_left ? this->_left : this->_right;
    }

    const binary_node* last_child() const {
        return this->_right ? this->_right : this->_left;
    }

    bool is_left_child() const {
        return this->_parent ? this == this->_parent->_left : false;
    }

    bool is_right_child() const {
        return this->_parent ? this == this->_parent->_right : false;
    }

    bool is_unique_child() const {
        return this == first_child() && this == last_child();
    }

    long hash_code() const;

    template <
        typename ConvertibleT = T,
        CHECK_CONVERTIBLE(ConvertibleT, T)>
    bool operator==(const binary_node<ConvertibleT>& other) const {
        // Trivial case exclusion (for performance reason).
        if ((this->_left == nullptr) != (other._left == nullptr)
            || (this->_right == nullptr) != (other._right == nullptr)) {
            return false;
        }
        // Test value for inequality.
        if (this->_value != other._value) {
            return false;
        }
        // Deep comparison (at this point both are either null or something, only on test before access is enough).
        if (this->_left && *this->_left != *other._left) {
            return false;
        }
        // Deep comparison (at this point both are either null or something, only on test before access is enough).
        if (this->_right && *this->_right != *other._right) {
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
        CHECK_CONVERTIBLE(ConvertibleT, T)>
    bool operator==(const temp_node<ConvertibleT>& other) const {
        // Too large tree
        if (other.children_number() > 2) {
            return false;
        }
        // All the possible false cases were tested, then it's true.
        return true;
    }

    /*   ---   Tree construction   ---   */
    void replace_with(binary_node& node) {
        if (this->_parent) {
            node._parent = this->_parent;
            if (this ==this->_parent->_left) {
                this->_parent->_left = &node;
            }
            if (this ==this->_parent->_right) {
                this->_parent->_right = &node;
            }
            this->_parent = nullptr;
        }
    }
};

} // namespace ds
