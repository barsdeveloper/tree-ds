#pragma once

#include <TreeDS/node.hpp>
#include <cstddef> // nullptr_t
#include <string>
#include <type_traits>

namespace ds {

template <typename T>
class temporary_node : public node<T> {

    template <typename X>
    friend temporary_node<X> n(X);
    friend temporary_node<std::string> n(const char*);

private:
    size_t _tree_size;

public:
    temporary_node(T value) :
        node<T>(value),
        _tree_size(1) {
    }

    size_t get_size() const {
        return _tree_size;
    }

    temporary_node operator()(temporary_node&& left, std::nullptr_t right = nullptr) {
        this->_tree_size += left._tree_size;
        this->set_left(std::make_unique<node<T>>(std::move(left)));
        return std::move(*this);
    }

    temporary_node operator()(temporary_node&& left, temporary_node&& right) {
        this->_tree_size += left._tree_size + right._tree_size;
        this->set_left(std::make_unique<node<T>>(std::move(left)));
        this->set_right(std::make_unique<node<T>>(std::move(right)));
        return std::move(*this);
    }

    temporary_node operator()(std::nullptr_t left, temporary_node&& right) {
        this->_tree_size += right._tree_size;
        this->set_right(std::make_unique<node<T>>(std::move(right)));
        return std::move(*this);
    }
};

} /* namespace ds */
