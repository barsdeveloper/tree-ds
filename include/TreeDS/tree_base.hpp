#pragma once

#include <TreeDS/binary_node.hpp>
#include <cstddef> // std::size_t
#include <memory>  // std::unique_ptr

namespace ds {

template <typename, typename, bool>
class tree_iterator;

/**
 * @brief Base class for {@link tree}. Do not use it directly.
 * @details The purpose of this class is to have a type that depends only on the T template parameter. As iterators need
 * to keep a pointer to this class (to obtain the {@link #_root root node}), all the iterators of this container can
 * depend only on the value type (T) (this is known as scary iterator). Doing so you can assign iterators of
 * different container type (but with the same T template parameter, e.g.: different allocator) to each other.
 * REMEMBER: this is not required by the standard: you shouldn't rely on this behavior for other STL containers.
 */
template <typename T>
class tree_base {

    template <typename, typename, bool>
    friend class tree_iterator;
    template <typename U>
    void swap(tree_base<U>& a, tree_base<U>& b);

public:
    using value_type      = T;
    using reference       = value_type&;
    using const_reference = const value_type&;
    using node_type       = binary_node<T>;
    using size_type       = std::size_t;
    using difference_type = std::ptrdiff_t;

protected:
    /// @brief Owning pointer to the root node.
    node_type* _root;
    /// @brief The number of nodes in the tree.
    size_type _size;

protected:
    /**
	 * @brief Create an empty tree.
	 */
    constexpr tree_base() :
            _root(nullptr),
            _size(0) {
    }

    /**
	 * @brief Create a tree by passing root and size information.
	 * @param root an owning pointer to the root node
	 * @param size the size of the tree
	 */
    tree_base(node_type* root, size_type size) :
            _root(root),
            _size(size) {
    }

    // Memory mess up logic is implement in subclass, no particular reason, that's my will, muhahahahaha.
    tree_base(const tree_base& other) = delete;
    tree_base(tree_base&& other)      = delete;
    tree_base& operator=(const tree_base&) = delete;
    tree_base& operator=(tree_base&&) = delete;

    void nullify() {
        _root = nullptr;
        _size = 0;
    }

    ~tree_base() = default;

public:
    /**
	 * @brief Checks whether the container is empty.
	 * @details If a tree is empty then:
	 * <ul>
	 *     <li>It doesn't have a root.</li>
	 *     <li>It has a {@link #size()} of 0.</li>
	 *     <li>{@link tree#begin() begin()} == {@link tree#end() end()}.</li>
	 * </ul>
	 * @return true if the tree is empty
	 */
    bool empty() const {
        return _root == nullptr;
    }

    /**
	 * @brief Returns the number of the nodes in this tree
	 * @return the number of nodes
	 */
    size_type size() const {
        return _size;
    }

    /**
	 * @brief Returns the maximum possible number of elements the tree can hold.
	 * @details Please note that this is not the real value because the size of the tree will be more likely limited by
	 * the amount of free memory. You never want to use this method, it is stupid, it exists just to say that this
	 * library is stl-like.
	 * @return maximum possible number of elements
	 */
    size_type max_size() const {
        return std::numeric_limits<size_type>::max();
    }

    /**
	 * @brief Performs a deep comparison of this tree with the other
	 * @param other the tree to compare with
	 * @return true if the trees are lexicographically equal
	 */
    bool operator==(const tree_base& other) const {
        // 1. Test if different size
        if (this->_size != other._size) {
            return false;
        }
        // 2. Test if one between this or other has a root that is set while the other doesn't.
        if ((this->_root == nullptr) != (other._root == nullptr)) {
            return false;
        }
        // At the end is either null (both) or same as the other.
        return this->_root == nullptr || *this->_root == *other._root;
    }

    /**
	 * Performs a deep comparison of this tree with the other.
	 * @param other the tree to compare with
	 * @return true if the trees are <b>not</b> lexicographically equal
	 */
    bool operator!=(const tree_base& other) const {
        return !(*this == other);
    }

    /**
	 * @brief Swaps the contents of this tree with those of the other.
	 * @details After this method will be called, this tree will have the root and size of the other tree while the
	 * other tree will have root and size of this tree. All references to elements remain valid. Iterators however will
	 * not. Do not use iterator after you swapped trees.
	 * @param other the tree to swap with
	 */
    void swap(tree_base<T>& other) {
        // I want unqualified name lookup in order to let invoking an user-defined swap function outside std namespace.
        using namespace std;
        swap(_root, other._root);
        swap(_size, other._size);
    }
};

} // namespace ds
