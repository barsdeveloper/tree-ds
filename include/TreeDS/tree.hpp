#pragma once

#include <iterator>    // std::make_reverse_iterator
#include <stdexcept>   // std::logic_error
#include <tuple>       // make_from_tuple
#include <type_traits> // std::enable_if
#include <utility>     // std::move(), std::forward()

#include <TreeDS/basic_tree.hpp>
#include <TreeDS/node/struct_node.hpp>
#include <TreeDS/policy/post_order.hpp>
#include <TreeDS/policy/pre_order.hpp>
#include <TreeDS/tree_iterator.hpp>

namespace md {

/**
 * @brief A generic tree class that uses a custom node type.
 * @details A binary tree structured as a hierarchy of {@link node}. Every node
 * keeps an object of type T and can have a most two nodes as children. This
 * class is compatible with STL containers. For example you can insert an
 * element into the data structure by calling the usual:<br />
 * <ul>
 *     <li>{@link #insert(It, const_reference) insert(iterator, const T&)} - if you want copy semantics</li>
 *     <li>{@link #insert(It, value_type&&) insert(iterator, T&&)} - if you want move semantics</li>
 * </ul>
 * method and using an {@link #iterator} as place indicator.
 *
 * @tparam T type of element hold by the tree
 * @tparam Policy default iterator algorithm used to traverse the tree in a range-based for loop
 * @tparam Allocator allocator type used to allocate new nodes
 *
 * @sa binary_tree
 */
template <
    typename T,
    typename Node,
    typename Policy,
    typename Allocator>
class tree : public basic_tree<T, Node, Policy, Allocator> {

    template <typename, typename, typename, typename>
    friend class tree;

    using super = basic_tree<T, Node, Policy, Allocator>;

    public:
    //   ---   TYPES   ---
    DECLARE_TREEDS_TYPES(T, Node, Policy, Allocator)

    static_assert(
        is_tag_of_policy<Policy, Node, allocator_type>,
        "Policy template parameter is expected to be an actual policy tag");

    protected:
    tree(node_type* root) :
            super(root, 0u, 0u) {
    }

    tree(node_type* root, size_type size, size_type arity) :
            super(root, size, arity) {
    }

    template <typename Alloc>
    tree(node_type* root, size_type size, size_type arity, Alloc&& allocator) :
            super(root, size, arity, std::forward<Alloc>(allocator)) {
    }

    public:
    //   ---   CONSTRUCTORS   ---
    /**
     * Create an empty tree.
     */
    constexpr tree() {
    }

    explicit tree(const Allocator& allocator) :
            super(allocator) {
    }

    /**
     * Copy constructor. Create a tree by copying the tree passed as argument. Remember that this involves deep copy
     * (use std::move() whenever you can). A new {@link node_type} will be allocated for each node of the other tree.
     * The allocator will be used.
     * @param other the tree to be copied from
     */
    explicit tree(const tree& other) :
            tree(
                other.root
                    ? allocate(this->allocator, *other.root, this->allocator).release()
                    : nullptr,
                other.size_value,
                other.arity_value) {
        static_assert(
            std::is_copy_constructible_v<T>,
            "Tried to COPY a tree containing a non copyable type.");
    }

    template <typename OtherPolicy>
    explicit tree(const tree<T, Node, OtherPolicy, Allocator>& other) :
            tree(
                other.root
                    ? allocate(this->allocator, *other.root, this->allocator).release()
                    : nullptr,
                other.size_value,
                other.arity_value,
                other.allocator) {
        static_assert(
            std::is_copy_constructible_v<T>,
            "Tried to COPY a tree containing a non copyable type.");
    }

    tree(tree&& other) :
            tree(other.root, other.size_value, other.arity_value, std::move(other.allocator)) {
        other.nullify();
    }

    /**
     * Other Policy move constructor. Create a tree by movings the tree passed as argument. The tree passed as argument
     * will be empty after this operation.
     * @param other the tree to be moved
     */
    template <typename OtherPolicy>
    tree(tree<T, Node, OtherPolicy, Allocator>&& other) :
            tree(other.root, other.size_value, other.arity_value, std::move(other.allocator)) {
        other.nullify();
    }

    tree(std::unique_ptr<Node, deleter<Allocator>> root, size_type size = 0u, size_type arity = 0u) :
            tree(root.release(), size, arity) {
    }

    /**
     * Create a tree by copying the nodes structure starting from the {@link temporary_node} passed as argument. A new
     * {@link node_type} will be allocated for each node in the structure passed. The allocation will use the allocator.
     * @param root the root of the newly created tree
     */
    template <
        typename ConvertibleT,
        typename... Children,
        CHECK_CONVERTIBLE(ConvertibleT, T)>
    tree(const struct_node<ConvertibleT, Children...>& root) :
            tree(
                allocate(this->allocator, root, this->allocator).release(), // root
                root.get_subtree_size(),                                    // size
                root.get_subtree_arity()) {                                 // arity
    }

    template <
        typename... EmplacingArgs,
        typename... Children,
        CHECK_CONSTRUCTIBLE(value_type, EmplacingArgs...)>
    tree(const struct_node<std::tuple<EmplacingArgs...>, Children...>& root) :
            tree(
                allocate(this->allocator, root, this->allocator).release(), // root
                root.get_subtree_size(),                                    // size
                root.get_subtree_arity()) {                                 // arity
    }

    /**
     * Destruct all the nodes and deallocate the memory owned by this tree.
     */
    ~tree() {
        clear();
    }

    public:
    //   ---   ASSIGNMENT   ---
    // Copy assignment
    tree& operator=(const tree& other) {
        return this->operator=<Policy>(other);
    }

    // Any other policy copy assignment
    template <typename OtherPolicy>
    tree& operator=(const basic_tree<T, Node, OtherPolicy, Allocator>& other) {
        static_assert(
            std::is_copy_assignable_v<T>,
            "Tried to COPY ASSIGN a tree containing a non copyable type.");
        this->assign(
            !other.empty()
                ? allocate(this->allocator, *other.root, this->allocator).release()
                : nullptr,
            other.size_value,
            other.arity_value);

        return *this;
    }

    // Move assignment
    tree& operator=(tree&& other) {
        return this->operator=<Policy>(std::move(other));
    }

    // Any other policy move assignment
    template <typename OtherPolicy>
    tree& operator=(tree<T, Node, OtherPolicy, Allocator>&& other) {
        this->allocator = std::move(other.allocator);
        this->assign(other.root, other.size_value, other.arity_value);
        other.nullify();
        return *this;
    }

    template <
        typename ConvertibleT,
        typename... Nodes,
        CHECK_CONVERTIBLE(ConvertibleT, T)>
    tree& operator=(const struct_node<ConvertibleT, Nodes...>& root) {
        this->assign(
            allocate(this->allocator, root, this->allocator).release(),
            root.get_subtree_size(),
            root.get_subtree_arity());
        return *this;
    }

    template <
        typename... EmplacingArgs,
        typename... Children,
        CHECK_CONSTRUCTIBLE(value_type, EmplacingArgs...)>
    tree& operator=(const struct_node<std::tuple<EmplacingArgs...>, Children...>& root) {
        this->assign(
            allocate(this->allocator, root, this->allocator).release(),
            root.get_subtree_size(),
            root.get_subtree_arity());
        return *this;
    }

    tree& operator=(std::unique_ptr<Node, deleter<allocator_type>> root) {
        this->assign(root.release(), 0u, 0u);
        return *this;
    }

    //   ---   ITERATORS   ---
    using super::begin;
    using super::end;
    using super::rbegin;
    using super::rend;

    /**
     * @brief Returns an iterator to the beginning.
     * @details The iterator will point to the first element of the container. Which one is the first depends on the
     * algorithm used to traverse the tree.
     * @return iterator to the first element
     */
    template <typename P = Policy>
    iterator<P> begin(P = Policy()) {
        // Incremented to shift it to the first element (initially it's end-equivalent)
        return ++iterator<P>(*this);
    }

    /**
     * @brief Returns an iterator to the end.
     *
     * The iterator will point one step after the last element of the container. Which one is the last depends on the
     * algorithm used to traverse the tree. It's perfectly legitimate to decrement this iterator to obtain the last
     * element.
     * @return iterator the element following the last element
     */
    template <typename P = Policy>
    iterator<P> end(P = Policy()) {
        return iterator<P>(*this);
    }

    template <typename P = Policy>
    reverse_iterator<P> rbegin(P policy = Policy()) {
        return std::make_reverse_iterator(this->end(policy));
    }

    // reverse end
    template <typename P = Policy>
    reverse_iterator<P> rend(P policy = Policy()) {
        // Incremented to shift it to the first element (initially it's end-equivalent)
        return std::make_reverse_iterator(this->begin(policy));
    }

    //   ---   GETTERS   ---
    public:
    using super::get_root;

    Node* get_root() {
        return this->root;
    }

    //   ---   MODIFIERS   ---
    protected:
    std::unique_ptr<node_type, deleter<allocator_type>> replace_node(
        node_type* replaced,
        node_type* replacement,
        size_type replacement_size,
        size_type replacement_arity) {
        if (replaced != nullptr) {
            assert(this->root != nullptr);
            replaced->replace_with(replacement);
            if (replaced == this->root) {
                this->size_value  = replacement_size;
                this->arity_value = replacement_arity;
            } else if (!replaced->has_children()) {
                this->size_value += replacement_size - 1;
                this->arity_value = 0u; // we don't have useful information
            } else {
                this->size_value  = 0u;
                this->arity_value = 0u;
            }
        }
        return {replaced, deleter(this->allocator)};
    }

    template <typename P, bool Constant>
    iterator<P> modify_subtree(
        tree_iterator<super, P, Constant> position,
        std::unique_ptr<node_type, deleter<allocator_type>> node,
        size_type replacement_size,
        size_type replacement_arity) {
        if (!this->is_own_iterator(position)) {
            throw std::logic_error("Tried to modify the tree (insert or emplace) with an iterator not belonging to it.");
        }
        node_type* replacement = node.release();
        /*
         * const_cast is needed here because we must accept a tree::const_iterator which treats nodes as constant. That
         * iterator refers to this tree, which is however mutable, just like the node that iterator is pointing to,
         * making it safe to cast away the const-ness. I personally hate to see const_cast in the code but there is no
         * efficient work around except crazy approaches like traversing the whole tree starting from the root to
         * retrieve a non constant version of the value we already have.
         */
        node_type* target = const_cast<node_type*>(position.get_node());
        if (target != nullptr) { // if iterator points to valid node
            assert(this->root != nullptr);
            position.update(*target, replacement);
            if (target == this->root) {
                this->assign(replacement, replacement_size, replacement_arity);
            } else {
                this->replace_node(target, replacement, replacement_size, replacement_arity);
            }
        } else if (this->root == nullptr) {
            this->assign(replacement, replacement_size, replacement_arity);
        } else {
            throw std::logic_error("The iterator points to a non valid position (end).");
        }
        if constexpr (Constant) {
            return position.craft_non_constant_iterator();
        } else {
            return position;
        }
    }

    template <bool First, typename P, bool C>
    iterator<P> add_child(
        tree_iterator<super, P, C> position,
        std::unique_ptr<node_type, deleter<allocator_type>> node,
        size_type replacement_size,
        size_type replacement_arity) {
        if (!this->is_own_iterator(position)) {
            throw std::logic_error("Tried to modify the tree (insert or emplace) with an iterator not belonging to it.");
        }
        /*
         * const_cast is needed here because we must accept a tree::const_iterator which treats nodes as constant. That
         * iterator refers to this tree, which is however mutable, just like the node that iterator is pointing to,
         * making it safe to cast away the const-ness. I personally hate to see const_cast in the code but there is no
         * efficient work around except crazy approaches like traversing the whole tree starting from the root to
         * retrieve a non constant version of the value we already have.
         */
        node_type* target = const_cast<node_type*>(position.get_node());
        if (target == nullptr) {
            throw std::logic_error("The iterator points to a non valid position (end).");
        }
        if constexpr (std::is_same_v<std::decay_t<node_type>, binary_node<T>>) {
            if (target->children() == 2u) {
                throw std::logic_error("Tried to add a children to a binary_node with 2 children.");
            }
        }
        if constexpr (First) {
            target->prepend_child(node.release());
        } else {
            target->append_child(node.release());
        }
        this->size_value += replacement_size;
        // +1 because get_following_siblings() counts just the node that follow next
        std::size_t local_arity = target->get_first_child()
            ? target->get_first_child()->get_following_siblings() + 1
            : 1u;
        this->arity_value = std::max(replacement_arity, std::max(this->arity_value, local_arity));
        return iterator<P>(this, target);
    }

    template <typename P, bool Constant>
    void erase_subtree(tree_iterator<super, P, Constant> position) {
        /*
         * const_cast is needed here because we must accept a tree::const_iterator which treats nodes as constant. That
         * iterator refers to this tree, which is however mutable, just like the node that iterator is pointing to,
         * making it safe to cast away the const-ness. I personally hate to see const_cast in the code but there is no
         * efficient work around except crazy approaches like traversing the whole tree starting from the root to
         * retrieve a non constant version of the value we already have.
         */
        node_type* target = const_cast<node_type*>(position.get_node());
        if (target != nullptr) { // if iterator points to valid node
            assert(this->root != nullptr);
            position.update(*target, nullptr);
            if (target == this->root) {
                this->clear();
            } else {
                this->replace_node(target, nullptr, 0u, 0u);
            }
        } else if (this->root != nullptr) {
            throw std::logic_error("The iterator points to a non valid position (end).");
        }
    }

    void assign(node_type* root, size_type size, size_type arity) {
        if (this->root != nullptr) {
            deallocate(this->allocator, this->root);
        }
        this->root        = root;
        this->size_value  = size;
        this->arity_value = arity;
    }

    void nullify() {
        this->root        = nullptr; // deallocation was already node somewhere else
        this->size_value  = 0u;
        this->arity_value = 0u;
    }

    public:
    /**
     * @brief Removes all elements from the tree.
     * @details Invalidates any references, pointers, or iterators referring to contained elements. Any past-the-last
     * element iterator ({@link tree#end() end()}) remains valid.
     */
    void clear() {
        this->assign(nullptr, 0u, 0u);
    }

    /**
     * @brief Swaps the contents of this tree with those of the other.
     * @details After this method will be called, this tree will have the root and size of the other tree while the
     * other tree will have root and size of this tree. All references to elements remain valid. Iterators however will
     * not. Do not use iterator after you swapped trees.
     * @param other the tree to swap with
     */
    template <typename OtherPolicy>
    void swap(tree<T, Node, OtherPolicy, Allocator>& other) {
        // I want unqualified name lookup in order to let invoking an user-defined swap function outside std namespace.
        using namespace std;
        std::swap(this->root, other.root);
        std::swap(this->size_value, other.size_value);
        std::swap(this->arity_value, other.arity_value);
    }

    /**
     * Insert an element at the specified position by replacing the existent
     * node. This will use the copy semantics.
     * @param position where to insert the element
     * @param value to insert in the tree, that will be copied
     * @return an iterator that points to the inserted element
     */
    template <typename P, bool C>
    iterator<P> insert_over(
        const tree_iterator<super, P, C>& position,
        const T& value) {
        return this->modify_subtree(position, allocate(this->allocator, value), 1u, 0u);
    }

    /**
     * @overload
     */
    template <typename P, bool C>
    iterator<P> insert_over(
        const tree_iterator<super, P, C>& position,
        T&& value) {
        return this->modify_subtree(position, allocate(this->allocator, std::move(value)), 1u, 0u);
    }

    /**
     * Insert an element at the specified position by replacing the existent
     * node. This will use the copy semantics.
     * @param position where to insert the element
     * @param node an r-value reference to a temporary_node
     * @return an iterator that points to the inserted element
     */
    template <
        typename P,
        bool C,
        typename ConvertibleT,
        typename... Children,
        CHECK_CONVERTIBLE(ConvertibleT, value_type)>
    iterator<P> insert_over(
        const tree_iterator<super, P, C>& position,
        const struct_node<ConvertibleT, Children...>& node) {
        return this->modify_subtree(
            position,
            // last allocator is forwarded to Node constructor to allocate its children
            allocate(this->allocator, node, this->allocator),
            node.get_subtree_size(),
            node.get_subtree_arity());
    }

    template <typename P, bool C, typename OtherP>
    iterator<P> insert_over(
        const tree_iterator<super, P, C>& position,
        const basic_tree<T, Node, OtherP, Allocator>& other) {
        return this->modify_subtree(
            position,
            // last allocator is forwarded to Node constructor to allocate its children
            !other.empty()
                ? allocate(this->allocator, *other.get_root(), this->allocator)
                : nullptr,
            other.size(),
            other.arity());
    }

    template <typename P, bool C, typename OtherP>
    iterator<P> insert_over(
        const tree_iterator<super, P, C>& position,
        tree<T, Node, OtherP, Allocator>&& other) {
        if constexpr (std::is_same_v<policy_type, OtherP>) {
            if (this == &other) {
                throw std::logic_error("Cannot move from yourself and create a recursive tree.");
            }
        }
        iterator<P> result = this->modify_subtree(
            position,
            other.replace_node(other.root, nullptr, 0u, 0u),
            other.size(),
            other.arity());
        other.nullify();
        return result;
    }

    template <
        typename P,
        bool C,
        typename... Args,
        CHECK_CONSTRUCTIBLE(T, Args...)>
    iterator<P> emplace_over(
        const tree_iterator<super, P, C>& position,
        Args&&... args) {
        return this->modify_subtree(
            position,
            allocate(this->allocator, std::forward<Args>(args)...),
            1u,
            0u);
    }

    template <
        typename P,
        bool C,
        typename... EmplacingArgs,
        typename... Children,
        CHECK_CONSTRUCTIBLE(value_type, EmplacingArgs...)>
    iterator<P> emplace_over(
        const tree_iterator<super, P, C>& position,
        const struct_node<std::tuple<EmplacingArgs...>, Children...>& node) {
        return this->modify_subtree(
            position,
            // last allocator is forwarded to Node constructor to allocate its children
            allocate(this->allocator, node, this->allocator),
            node.get_subtree_size(),
            node.get_subtree_arity());
    }

    template <typename P, bool C>
    iterator<P> insert_child_front(
        const tree_iterator<super, P, C>& position,
        T& value) {
        return this->add_child<true>(position, allocate(this->allocator, value), 1u, 0u);
    }

    template <typename P, bool C>
    iterator<P> insert_child_front(
        const tree_iterator<super, P, C>& position,
        T&& value) {
        return this->add_child<true>(position, allocate(this->allocator, std::move(value)), 1u, 0u);
    }

    template <
        typename P,
        bool C,
        typename ConvertibleT,
        typename... Children,
        CHECK_CONVERTIBLE(ConvertibleT, T)>
    iterator<P> insert_child_front(
        const tree_iterator<super, P, C>& position,
        struct_node<ConvertibleT, Children...> value) {
        // last allocator is forwarded to Node constructor to allocate its children
        return this->add_child<true>(position, allocate(this->allocator, value, this->allocator), 1u, 0u);
    }

    template <typename P, bool C, typename OtherP>
    iterator<P> insert_child_front(
        const tree_iterator<super, P, C>& position,
        const basic_tree<T, Node, OtherP, Allocator>& other) {
        return this->add_child<true>(
            position,
            !other.empty()
                ? allocate(this->allocator, *other.get_root(), this->allocator)
                : nullptr,
            1u,
            0u);
    }

    template <typename P, bool C, typename OtherP>
    iterator<P> insert_child_front(
        const tree_iterator<super, P, C>& position,
        tree<T, Node, OtherP, Allocator>&& other) {
        if constexpr (std::is_same_v<policy_type, OtherP>) {
            if (this == &other) {
                throw std::logic_error("Cannot move from yourself and create a recursive tree.");
            }
        }
        iterator<P> result = this->add_child<true>(
            position,
            other.replace_node(other.root, nullptr, 0u, 0u),
            1u,
            0u);
        other.nullify();
        return result;
    }

    template <typename P, bool C>
    iterator<P> insert_child_back(
        const tree_iterator<super, P, C>& position,
        T& value) {
        return this->add_child<false>(position, allocate(this->allocator, value), 1u, 0u);
    }

    template <typename P, bool C>
    iterator<P> insert_child_back(
        const tree_iterator<super, P, C>& position,
        T&& value) {
        return this->add_child<false>(position, allocate(this->allocator, std::move(value)), 1u, 0u);
    }

    template <
        typename P,
        bool C,
        typename ConvertibleT,
        typename... Children,
        CHECK_CONVERTIBLE(ConvertibleT, T)>
    iterator<P> insert_child_back(
        const tree_iterator<super, P, C>& position,
        struct_node<ConvertibleT, Children...> value) {
        return this->add_child<false>(
            position,
            // last allocator is forwarded to Node constructor to allocate its children
            allocate(this->allocator, value, this->allocator),
            value.get_subtree_size(),
            value.get_subtree_arity());
    }

    template <typename P, bool C, typename OtherP>
    iterator<P> insert_child_back(
        const tree_iterator<super, P, C>& position,
        const basic_tree<T, Node, OtherP, Allocator>& other) {
        return this->add_child<false>(
            position,
            !other.empty()
                ? allocate(this->allocator, *other.get_root())
                : nullptr,
            1u,
            0u);
    }

    template <typename P, bool C, typename OtherP>
    iterator<P> insert_child_back(
        const tree_iterator<super, P, C>& position,
        tree<T, Node, OtherP, Allocator>&& other) {
        if constexpr (std::is_same_v<policy_type, OtherP>) {
            if (this == &other) {
                throw std::logic_error("Cannot move from yourself and create a recursive tree.");
            }
        }
        iterator<P> result = this->add_child<false>(
            position,
            other.replace_node(other.root, nullptr, 0u, 0u),
            1u,
            0u);
        other.nullify();
        return result;
    }

    template <
        typename P,
        bool C,
        typename... Args,
        CHECK_CONSTRUCTIBLE(T, Args&&...)>
    iterator<P> emplace_child_front(
        const tree_iterator<super, P, C>& position,
        Args&&... args) {
        return this->add_child<true>(position, allocate(this->allocator, std::forward<Args>(args)...), 1u, 0u);
    }

    template <
        typename P,
        bool C,
        typename... EmplacingArgs,
        typename... Children,
        CHECK_CONSTRUCTIBLE(value_type, EmplacingArgs...)>
    iterator<P> emplace_child_front(
        const tree_iterator<super, P, C>& position,
        const struct_node<std::tuple<EmplacingArgs...>, Children...>& node) {
        return this->add_child<true>(
            position,
            // last allocator is forwarded to Node constructor to allocate its children
            allocate(this->allocator, node, this->allocator),
            node.get_subtree_size(),
            node.get_subtree_arity());
    }

    template <
        typename P,
        bool C,
        typename... Args,
        CHECK_CONSTRUCTIBLE(T, Args&&...)>
    iterator<P> emplace_child_back(
        const tree_iterator<super, P, C>& position,
        Args&&... args) {
        return this->add_child<false>(position, allocate(this->allocator, std::forward<Args>(args)...), 1u, 0u);
    }

    template <
        typename P,
        bool C,
        typename... EmplacingArgs,
        typename... Children,
        CHECK_CONSTRUCTIBLE(value_type, EmplacingArgs...)>
    iterator<P> emplace_child_back(
        const tree_iterator<super, P, C>& position,
        const struct_node<std::tuple<EmplacingArgs...>, Children...>& node) {
        return this->add_child<false>(
            position,
            // last allocator is forwarded to Node constructor to allocate its children
            allocate(this->allocator, node, this->allocator),
            node.get_subtree_size(),
            node.get_subtree_arity());
    }

    iterator<policy::post_order> erase(const_iterator<policy::post_order> position) {
        if (position.pointed_tree != this) {
            throw std::logic_error("Tried to modify the tree (erase) with an iterator not belonging to it.");
        }
        iterator<policy::post_order> result(position.craft_non_constant_iterator());
        ++result;
        this->erase_subtree(position);
        return result;
    }

    iterator<policy::post_order> erase(
        const_iterator<policy::post_order> first,
        const_iterator<policy::post_order> last) {
        if (first.pointed_tree != this || last.pointed_tree != this) {
            throw std::logic_error("Tried to modify the tree (erase) with an iterator not belonging to it.");
        }
        while (first != last) {
            this->erase_subtree(first++);
        }
        return last.craft_non_constant_iterator();
    }

    template <typename P, bool C>
    tree<T, Node, Policy, Allocator>
    detach_subtree(const tree_iterator<super, P, C>& position) {
        if (position.pointed_tree != this) {
            throw std::logic_error("Tried to modify the tree (detach subtree) with an iterator not belonging to it.");
        }
        node_type* target = const_cast<node_type*>(position.get_node());
        if (target == nullptr) {
            throw std::logic_error("The iterator points to a non valid position (end).");
        }
        return {this->replace_node(target, nullptr, 0, 0).release()};
    }

    //  ---   COMPARISON   ---
    using super::operator==;
};

// Swappable requirement
template <
    typename T,
    typename Node,
    typename Policy1,
    typename Policy2,
    typename Allocator>
void swap(tree<T, Node, Policy1, Allocator>& lhs, tree<T, Node, Policy2, Allocator>& rhs) noexcept {
    return lhs.swap(rhs);
}

} // namespace md
