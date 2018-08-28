#pragma once

#include <TreeDS/node/binary_node.hpp>
#include <iterator>    // std::iterator
#include <type_traits> // std::conditional, std::enable_if

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
    algorithm_type algorithm;
    tree_type* iterated_tree; // nullptr => no container associated (default iterator)
    node_type* current_node;  // nullptr => end()

    tree_iterator(tree_type* tree, node_type* current = nullptr) :
            iterated_tree(tree),
            current_node(current) {
    }

    public:
    ~tree_iterator() = default;

    // Iterators must be default constructible
    constexpr tree_iterator() :
            algorithm(),
            iterated_tree(nullptr),
            current_node(nullptr) {
    }

    // Iterators must be CopyConstructible
    tree_iterator(const tree_iterator&) = default;
    // Iterators must be CopyAssignable
    tree_iterator& operator=(const tree_iterator&) = default;

    tree_iterator(tree_type& tree, bool go_first = true) :
            algorithm(),
            iterated_tree(&tree),
            current_node(nullptr) {
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
            algorithm(),
            iterated_tree(other.iterated_tree),
            current_node(other.current_node) {
    }

    template <typename OtherAlgorithm, bool OtherConstant, typename = std::enable_if<!Constant || OtherConstant>>
    tree_iterator& operator=(const tree_iterator<Tree, OtherAlgorithm, OtherConstant>& other) {
        iterated_tree = other.iterated_tree;
        current_node  = other.current_node;
        return *this;
    }

    node_type* get_node() const {
        return current_node;
    }

    value_type& operator*() const {
        return current_node->value;
    }

    value_type* operator->() const {
        return &(current_node->value);
    }

    template <bool OtherConst>
    bool operator==(const tree_iterator<Tree, Algorithm, OtherConst>& other) const {
        return iterated_tree == other.iterated_tree && current_node == other.current_node;
    }

    template <bool OtherConst>
    bool operator!=(const tree_iterator<Tree, Algorithm, OtherConst>& other) const {
        return !(*this == other);
    }

    tree_iterator& operator++() {
        if (current_node) {
            // const_cast needed in case node_type is non const
            current_node = const_cast<node_type*>(algorithm.increment(*current_node));
        } else if (iterated_tree && iterated_tree->root) {
            /*
             * If iterator is at the end():
             *     normal iterator  => incremented from end() => go to its first element (rewind)
             *     reverse iterator	=> decremented from end() => go to its last element (before end())
             * REMEMBER: ++ operator on a reverse_iterator delegates to -- operator of tree_iterator and vice versa
             */
            // const_cast needed in case node_type is non const
            current_node = const_cast<node_type*>(algorithm.go_first(*iterated_tree->root));
        }
        return *this;
    }

    tree_iterator operator++(int) {
        tree_iterator it(*this);
        ++(*this);
        return it;
    }

    tree_iterator& operator--() {
        if (current_node) {
            // const_cast needed in case node_type is non constant
            current_node = const_cast<node_type*>(algorithm.decrement(*current_node));
        } else if (iterated_tree && iterated_tree->root) {
            /*
			 * If iterator is at the end():
			 *     normal iterator  => decremented from end() => go to its last element (before end())
			 *     reverse iterator => incremented from end() => go to its first element (rewind)
			 * REMEMBER: ++ operator on a reverse_iterator delegates to -- operator of tree_iterator and vice versa
			 */
            // const_cast needed in case node_type is non const
            current_node = const_cast<node_type*>(algorithm.go_last(*iterated_tree->root));
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
