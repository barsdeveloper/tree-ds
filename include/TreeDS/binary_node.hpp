#pragma once

#include <TreeDS/temporary_node.hpp>
#include <TreeDS/utility.hpp>
#include <cassert> // assert
#include <memory>  // std::unique_ptr, std::move()
#include <variant>

namespace ds {

template <typename T>
class binary_node {

    template <typename, typename, bool>
    friend class tree_iterator;

    template <typename, typename, typename>
    friend class tree;

protected:
    T _value;
    binary_node<T>* _parent;
    binary_node<T>* _left;
    binary_node<T>* _right;

public:
    // Forward constructor: the arguments are forwarded directly to the constructor of the type T hold into this node.
    template <typename... Args, typename = std::enable_if_t<std::is_constructible_v<T, Args...>>>
    explicit binary_node(Args&&... args) :
            _value(std::forward<Args>(args)...),
            _parent(nullptr),
            _left(nullptr),
            _right(nullptr) {
    }

    // Value copy constructor
    explicit binary_node(const T& value) :
            _value(value),
            _parent(nullptr),
            _left(nullptr),
            _right(nullptr) {
    }

    template <
        typename ConvertibleT = T,
        typename AllocateFn,
        typename = std::enable_if_t<
            std::is_convertible_v<ConvertibleT, T>>>
    explicit binary_node(const binary_node<ConvertibleT>& other, AllocateFn allocate) :
            _value(static_cast<T>(other._value)),
            _parent(nullptr),
            _left(other._left ? attach(allocate(*other._left, allocate).release()) : nullptr),
            _right(other._right ? attach(allocate(*other._right, allocate).release()) : nullptr) {
    }

    // Construct from temporary_node using allocator
    template <
        typename ConvertibleT = T,
        typename AllocateFn,
        CHECK_CONVERTIBLE_T>
    explicit binary_node(const temporary_node<ConvertibleT>& other, AllocateFn allocate) :
            _value(std::move(other.get_value())),
            _parent(nullptr),
            _left(nullptr),
            _right(nullptr) {
        if (!other.has_children()) {
            return;
        }
        assert(other.children_number() <= 2);
        auto it = other.begin();
        {
            _left = std::holds_alternative<temporary_node<ConvertibleT>>(*it)
                ? attach(
                      allocate(
                          std::get<temporary_node<ConvertibleT>>(*it),
                          allocate)
                          .release())
                : nullptr;
            ++it;
        }
        if (it != other.end()) {
            _right = std::holds_alternative<temporary_node<ConvertibleT>>(*it)
                ? attach(
                      allocate(
                          std::get<temporary_node<ConvertibleT>>(*it),
                          allocate)
                          .release())
                : nullptr;
            ++it;
        }
        assert(it == other.end());
    }

    // Move constructor
    binary_node(binary_node&& other) :
            _value(other._value),
            _parent(other._parent),
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
        if (_parent) {
            if (_parent->_left == this) {
                _parent->_left = &node;
            }
            if (_parent->_right == this) {
                _parent->_right = &node;
            }
        }
        if (_left) {
            _left->_parent = &node;
        }
        if (_right) {
            _right->_parent = &node;
        }
        _parent = nullptr;
        _left   = nullptr;
        _right  = nullptr;
    }

public:
    /*   ---   Getters   ---   */
    const T value() const {
        return _value;
    }
    T value() {
        return _value;
    }

    const binary_node* parent() const {
        return _parent;
    }

    const binary_node* left_child() const {
        return _left;
    }

    const binary_node* right_child() const {
        return _right;
    }

    const binary_node* first_child() const {
        return _left ? _left : _right;
    }

    const binary_node* last_child() const {
        return _right ? _right : _left;
    }

    bool is_left_child() const {
        return _parent ? this == _parent->_left : false;
    }

    bool is_right_child() const {
        return _parent ? this == _parent->_right : false;
    }

    bool is_unique_child() const {
        return this == first_child() && this == last_child();
    }

    long hash_code() const;

    template <
        typename ConvertibleT = T,
        CHECK_CONVERTIBLE_T>
    bool operator==(const binary_node<ConvertibleT>& other) const {
        // Trivial case exclusion (for performance reason).
        if ((_left == nullptr) != (other._left == nullptr)
            || (_right == nullptr) != (other._right == nullptr)) {
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
        CHECK_CONVERTIBLE_T>
    bool operator!=(const binary_node<ConvertibleT>& other) const {
        return !(*this == other);
    }

    template <
        typename ConvertibleT = T,
        CHECK_CONVERTIBLE_T>
    bool operator==(const temporary_node<ConvertibleT>& other) const {
        // Too large tree
        if (other.children_number() > 2) {
            return false;
        }
        // All the possible false cases were tested, then it's true.
        return true;
    }

    /*   ---   Tree construction   ---   */
    void replace_with(binary_node& node) {
        if (_parent) {
            node._parent = _parent;
            if (this == _parent->_left) {
                _parent->_left = &node;
            }
            if (this == _parent->_right) {
                _parent->_right = &node;
            }
            _parent = nullptr;
        }
    }
};

} // namespace ds
