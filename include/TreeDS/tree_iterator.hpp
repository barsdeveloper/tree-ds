#pragma once

#include <iterator>    // std::iterator
#include <type_traits> // std::conditional, std::enable_if
#include "binary_node.hpp"

namespace ds {

template <typename, typename, typename>
class tree;

template <typename Tree, typename Algorithm, bool Constant = false>
class tree_iterator {

    template <typename, typename, typename>
    friend class tree;

    template <typename, typename, bool>
    friend class tree_iterator;

    public:
    using tree_type = std::conditional_t<
        Constant,
        const Tree,
        Tree>;
    using node_type = std::conditional_t<
        Constant,
        const typename tree_type::node_type,
        typename tree_type::node_type>;
    using algorithm_type = const Algorithm;
    // Iterators mandatory type declarations
    using value_type = std::conditional_t<
        Constant,
        const typename tree_type::value_type,
        typename tree_type::value_type>;
    using difference_type   = std::ptrdiff_t;
    using pointer           = value_type*;
    using reference         = value_type&;
    using iterator_category = std::bidirectional_iterator_tag;

    protected:
    algorithm_type _algorithm;
    tree_type* _tree;    // nullptr => no container associated (default iterator)
    node_type* _current; // nullptr => end()

    tree_iterator(tree_type* tree, node_type* current = nullptr) :
            _tree(tree),
            _current(current) {
    }

    public:
    ~tree_iterator() = default;

    // Iterators must be default constructible
    constexpr tree_iterator() :
            _algorithm(),
            _tree(nullptr),
            _current(nullptr) {
    }

    // Iterators must be CopyConstructible
    tree_iterator(const tree_iterator&) = default;
    // Iterators must be CopyAssignable
    tree_iterator& operator=(const tree_iterator&) = default;

    tree_iterator(tree_type& tree, bool go_first = true) :
            _algorithm(),
            _tree(&tree),
            _current(nullptr) {
        if (go_first) {
            ++(*this); // incrementing from _current = nullptr (which is end()), will take the iterator to the begin()
        }
    }

    /*   ---   Enlarged construction/assignment   ---   */
    /*
	 * This will enable conversion between templates with different a Algorithm but the same T. Also this allows a
	 * general conversion between any (but having the same parameter T) const_iterator to any iterator.
	 * Why explicit?
	 * Iterator must satisfy the multipass guarantee:
	 * http://en.cppreference.com/w/cpp/concept/ForwardIterator
	 * That means that it1 == it2 implies ++it1 == ++it2. Obviously this would be false if we allow implicit conversion
	 * between iterators with different algorithm types. That's the reason of explicit, to limit compiler magics.
	 */
    template <typename OtherAlgorithm, bool OtherConstant, typename = std::enable_if<!Constant || OtherConstant>>
    explicit tree_iterator(const tree_iterator<Tree, OtherAlgorithm, OtherConstant>& other) :
            _algorithm(),
            _tree(other.tree),
            _current(other.current) {
    }

    template <typename OtherAlgorithm, bool OtherConstant, typename = std::enable_if<!Constant || OtherConstant>>
    tree_iterator& operator=(const tree_iterator<Tree, OtherAlgorithm, OtherConstant>& other) {
        _tree    = other._tree;
        _current = other._current;
        return *this;
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
    bool operator==(const tree_iterator<Tree, Algorithm, OtherConst>& other) const {
        return _tree == other._tree && _current == other._current;
    }

    template <bool OtherConst>
    bool operator!=(const tree_iterator<Tree, Algorithm, OtherConst>& other) const {
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
            // const_cast needed in case node_type is non constant
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

} // namespace ds
