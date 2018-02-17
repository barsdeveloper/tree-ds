#pragma once

#include <TreeDS/temporary_node.hpp>
#include <memory> // std::unique_ptr, std::move()

namespace ds {

template <typename T>
class node {

    template <typename, typename, bool>
    friend class tree_iterator;

    template <typename, typename, typename>
    friend class tree;

protected:
    T _value;
    node<T>* _parent;
    std::unique_ptr<node> _left;
    std::unique_ptr<node> _right;

public:
    template <typename... Args>
    node(Args... args) :
        _value{args...},
        _parent(nullptr),
        _left(nullptr),
        _right(nullptr) {
    }

    node(T value) :
        _value(value),
        _parent(nullptr),
        _left(nullptr),
        _right(nullptr) {
    }

    node(const node& other) :
        _value(other._value),
        _parent(nullptr) {
        if (other._left) {
            // TODO use allocators
            this->set_left(std::make_unique<node>(*other._left));
        }
        if (other._right) {
            // TODO use allocators
            this->set_right(std::make_unique<node>(*other._right));
        }
    }

    // needed otherwise passing temporary_node will call the template version of constructor
    node(const temporary_node<T>& other) :
        node(static_cast<const node&>(other)) {
    }

    node(node&& other) :
        _value(other._value),
        _parent(other._parent),
        _left(std::move(other._left)),
        _right(std::move(other._right)) {
        if (_left) {
            _left->_parent = this;
        }
        if (_right) {
            _right->_parent = this;
        }
        other._parent = nullptr;
    }

    ~node() = default;

protected:
    void set_left(std::unique_ptr<node> n) {
        _left = std::move(n);
        if (_left) {
            _left->_parent = this;
        }
    }

    void set_right(std::unique_ptr<node> n) {
        _right = std::move(n);
        if (_right) {
            _right->_parent = this;
        }
    }

public:
    /*   ---   Getters   ---   */
    const T value() const {
        return _value;
    }
    T value() {
        return _value;
    }

    const node* parent() const {
        return _parent;
    }

    const node* left_child() const {
        return _left.get();
    }

    const node* right_child() const {
        return _right.get();
    }

    const node* first_child() const {
        return _left ? _left.get() : _right.get();
    }

    const node* last_child() const {
        return _right ? _right.get() : _left.get();
    }

    bool is_left_child() const {
        return _parent ? this == _parent->_left : false;
    }

    bool is_right_child() const {
        return _parent ? this == _parent->_right : false;
    }

    bool is_unique_child() const {
        return this == left_child() && this == last_child();
    }

    long hash_code() const;

    bool operator==(const node& other) {
        /* Case when one between this or other has a left or right that is set while the other doesn't. */
        if (
            (this->_left == nullptr) != (other._left == nullptr)
            || (this->_right == nullptr) != (other._right == nullptr)) {
            return false;
        }
        // Test value for inequality
        if (this->_value != other._value) {
            return false;
        }
        if (this->_left) {
            if (*this->_left != *other._left) {
                return false;
            }
        }
        if (this->_right) {
            if (*this->_right != *other._right) {
                return false;
            }
        }
        // all the reasons to return false were discarded
        return true;
    }

    bool operator!=(const node& other) {
        return !(this->operator ==(other));
    }

    /*   ---   Tree construction   ---   */
    std::unique_ptr<node> substitute_with(std::unique_ptr<node> n) {
        std::unique_ptr<node> result;
        if (_parent) {
            n->_parent = _parent;
            if (this == _parent->_left.get()) {
                result         = std::move(_parent->_left);
                _parent->_left = std::move(n);
            } else {
                result          = std::move(_parent->_right);
                _parent->_right = std::move(n);
            }
            _parent = nullptr;
        }
        return std::move(result);
    }
};

} /* namespace ds */
