#pragma once

#include <iterator>    // std::iterator
#include <type_traits> // std::conditional, std::enable_if

#include <TreeDS/node.hpp>

namespace ds {

template <typename>
class tree_base;
template <typename, typename, typename>
class tree;
template <typename>
class node;

template <typename T, typename Algorithm, bool Constant = false>
class tree_iterator : public std::iterator<std::bidirectional_iterator_tag, T> {

    // const and non const are each other friends
    template <typename, typename, bool>
    friend class tree_iterator;
    template <typename, typename, typename>
    friend class tree;

public:
    using value_type     = typename std::conditional<Constant, const T, T>::type;
    using node_type      = typename std::conditional<Constant, const node<T>, node<T>>::type;
    using tree_type      = typename std::conditional<Constant, const tree_base<T>, tree_base<T>>::type;
    using algorithm_type = const Algorithm;

protected:
    algorithm_type _algorithm;
    tree_type* _tree;    // nullptr => no container associated (default iterator)
    node_type* _current; // nullptr => end()

    tree_iterator(tree_type* tree, node_type* current = nullptr) :
        _tree(tree),
        _current(current) {
    }

public:
    constexpr tree_iterator() :
        _algorithm(),
        _tree(nullptr),
        _current(nullptr) {
    }

    tree_iterator(const tree_iterator&) = default;

    /*
	 * This will enable conversion between templates with different Algorithm
	 * but same T. Also this allows a general conversion between any (but having
	 * same parameter T) const_iterator to any iterator.
	 * Why explicit?
	 * Iterator must satisfy the multipass guarantee:
	 * http://en.cppreference.com/w/cpp/concept/ForwardIterator
	 * That means that it1 == it2 implies ++it1 == ++it2. Clearly this would be
	 * false if we allow implicit conversion between iterators with different
	 * types. That's the reason of explicit, to limit compiler magics.
	 */
    template <
        typename OtherAlgorithm,
        bool OtherConstant,
        typename = std::enable_if<!Constant || OtherConstant>>
    explicit tree_iterator(
        const tree_iterator<T, OtherAlgorithm, OtherConstant>& other) :
        _algorithm(),
        _tree(other.tree),
        _current(other.current) {
    }

    tree_iterator& operator=(const tree_iterator& other) {
        _tree    = other._tree;
        _current = other._current;
        return *this;
    }

    ~tree_iterator() = default;

    tree_iterator(tree_type& tree, bool go_first = true) :
        _algorithm(),
        _tree(&tree),
        _current(nullptr) {
        if (go_first) {
            ++(*this);
        }
    }

    node_type* get_node() const {
        return _current;
    }

    value_type& operator*() const {
        return _current->_value;
    }

    value_type* operator->() const {
        return &(_current->_value);
    }

    template <bool OtherConst>
    bool operator==(
        const tree_iterator<T, Algorithm, OtherConst>& other) const {
        return _tree == other._tree && _current == other._current;
    }

    template <bool OtherConst>
    bool operator!=(
        const tree_iterator<T, Algorithm, OtherConst>& other) const {
        return !(*this == other);
    }

    tree_iterator& operator++() {
        if (_current) {
            // const_cast needed in case node_type is non const
            _current = const_cast<node_type*>(_algorithm.increment(*_current));
        } else if (_tree && _tree->_root) {
            /*
			 * If iterator is at the end():
			 *     normal iterator  => incremented from end() => go to its first element (rewind)
			 *     reverse iterator	=> decremented from end() => go to its last element (before end())
			 * REMEMBER: ++ operator on a reverse_iterator delegates to -- operator of tree_iterator and vice versa
			 */
            // const_cast needed in case node_type is non const
            _current = const_cast<node_type*>(_algorithm.go_first(*_tree->_root));
        }
        return *this;
    }

    tree_iterator operator++(int) {
        tree_iterator it(*this);
        ++(*this);
        return it;
    }

    tree_iterator& operator--() {
        if (_current) {
            node_type* temp = _current;
            // const_cast needed in case node_type is non const
            _current = const_cast<node_type*>(_algorithm.decrement(*_current));
        } else if (_tree && _tree->_root) {
            /*
			 * If iterator is at the end():
			 *     normal iterator  => decremented from end() => go to its last element (before end())
			 *     reverse iterator => incremented from end() => go to its first element (rewind)
			 * REMEMBER: ++ operator on a reverse_iterator delegates to -- operator of tree_iterator and vice versa
			 */
            // const_cast needed in case node_type is non const
            _current = const_cast<node_type*>(_algorithm.go_last(*_tree->_root));
        }
        return *this;
    }

    tree_iterator operator--(int) {
        tree_iterator it(*this);
        --(*this);
        return it;
    }
};

} /* namespace ds */
