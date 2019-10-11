#pragma once

#include <functional> // std::mem_fn()
#include <tuple>
#include <utility> // std::move(), std::forward()

#include <TreeDS/allocator_utility.hpp>
#include <TreeDS/node/binary_node.hpp>
#include <TreeDS/node/node.hpp>
#include <TreeDS/node/struct_node.hpp>
#include <TreeDS/utility.hpp>

namespace md {

template <typename T>
class nary_node : public node<T, nary_node<T>> {

    /*   ---   FRIENDS   ---   */
    template <typename, typename, typename>
    friend class tree_iterator;

    template <typename, typename, typename>
    friend class tree;

    template <typename, typename, typename, typename>
    friend class generative_navigator;

    /*   ---   ATTRIBUTES   ---   */
    protected:
    std::size_t following_size = 0u;
    nary_node* prev_sibling    = nullptr;
    nary_node* next_sibling    = nullptr;
    nary_node* first_child     = nullptr;
    nary_node* last_child; // It will be set by manage_parent_last_child() <- every constructor must call this method

    /*   ---   CONSTRUCTORS   ---   */
    public:
    // Forward constructor: the arguments are forwarded directly to the constructor of the type T
    template <typename... Args, typename = std::enable_if_t<std::is_constructible_v<T, Args...>>>
    explicit nary_node(Args&&... args) :
            node<T, nary_node>(nullptr, std::forward<Args>(args)...) {
        this->manage_parent_last_child();
    }

    // Forward constructor: the arguments are forwarded directly to the constructor of the type T (packed as tuple)
    template <typename... Args, typename = std::enable_if_t<std::is_constructible_v<T, Args...>>>
    explicit nary_node(const std::tuple<Args...>& args_tuple) :
            node<T, nary_node>(nullptr, args_tuple) {
        this->manage_parent_last_child();
    }

    // Move Constructor
    explicit nary_node(nary_node&& other) :
            node<T, nary_node>(std::move(other.get_value())),
            following_size(other.following_size),
            prev_sibling(other.prev_sibling),
            next_sibling(other.next_sibling),
            first_child(other.first_child),
            last_child(other.last_child) {
        nary_node* current_child = this->first_child;
        while (current_child) {
            current_child->parent = this;
            current_child         = current_child->get_next_sibling();
        }
        this->manage_parent_last_child();
        other.parent         = nullptr;
        other.following_size = 0u;
        other.prev_sibling   = nullptr;
        other.next_sibling   = nullptr;
        other.first_child    = nullptr;
        other.last_child     = nullptr;
    }

    /*   ---   Copy constructor using allocator   ---   */
    template <typename Allocator = std::allocator<nary_node>>
    explicit nary_node(const nary_node& other, Allocator&& allocator = Allocator()) :
            nary_node(other, nullptr, nullptr, allocator) {
    }

    /*   ---   Delegated copy constructor using allocator   ---   */
    template <typename Allocator>
    nary_node(
        const nary_node& other,
        nary_node* parent,
        nary_node* prev_sibling,
        Allocator&& allocator) :
            node<T, nary_node>(parent, other.value),
            following_size(other.following_size),
            prev_sibling(prev_sibling),
            next_sibling(
                other.next_sibling != nullptr
                    ? allocate(allocator, *other.next_sibling, this->parent, this, allocator).release()
                    : nullptr),
            first_child(
                other.first_child != nullptr
                    ? allocate(allocator, *other.first_child, this, nullptr, allocator).release()
                    : nullptr) {
        this->manage_parent_last_child();
    }

    /*   ---   Converting constructor from binary_node using allocator   ---   */
    template <typename Allocator = std::allocator<nary_node>>
    explicit nary_node(const binary_node<T>& other, Allocator&& allocator = Allocator()) :
            nary_node(other, nullptr, nullptr, allocator) {
    }

    /*   ---   Delegated converting constructor from binary_node using allocator   ---   */
    template <typename Allocator>
    nary_node(
        const binary_node<T>& other,
        nary_node* parent,
        nary_node* prev_sibling,
        Allocator&& allocator) :
            node<T, nary_node>(parent, other.get_value()),
            following_size(other.following_siblings()),
            prev_sibling(prev_sibling),
            next_sibling(
                other.get_next_sibling()
                    ? allocate(allocator, *other.get_next_sibling(), this->parent, this, allocator).release()
                    : nullptr),
            first_child(
                other.has_children()
                    ? allocate(allocator, *other.get_first_child(), this, nullptr, allocator).release()
                    : nullptr) {
        this->manage_parent_last_child();
    }

    /*   ---   Converting constructor from struct_node using allocator   ---   */
    template <
        typename ConvertibleT,
        typename FirstChild,
        typename NextSibling,
        typename Allocator = std::allocator<nary_node>,
        typename           = std::enable_if_t<std::is_convertible_v<ConvertibleT, T>>>
    explicit nary_node(
        const struct_node<ConvertibleT, FirstChild, NextSibling>& other,
        Allocator&& allocator = Allocator()) :
            nary_node(other, nullptr, nullptr, allocator) {
    }

    /*   ---   Delegated converting constructor from struct_node using allocator   ---   */
    template <
        typename ConvertibleT,
        typename FirstChild,
        typename NextSibling,
        typename Allocator,
        typename = std::enable_if_t<std::is_convertible_v<ConvertibleT, T>>>
    explicit nary_node(
        const struct_node<ConvertibleT, FirstChild, NextSibling>& other,
        nary_node* parent,
        nary_node* prev_sibling,
        Allocator&& allocator) :
            node<T, nary_node>(parent, other.get_value()),
            following_size(std::decay_t<decltype(other)>::following_siblings()),
            prev_sibling(prev_sibling),
            next_sibling(
                [&]() {
                    if constexpr (is_empty<NextSibling>) {
                        return nullptr;
                    } else {
                        return allocate(allocator, other.get_next_sibling(), parent, this, allocator).release();
                    }
                }()),
            first_child([&]() {
                if constexpr (is_empty<FirstChild>) {
                    return nullptr;
                } else {
                    return allocate(allocator, other.get_first_child(), this, nullptr, allocator).release();
                }
            }()) {
        this->manage_parent_last_child();
    }

    /*   ---   Converting constructor from emplacing struct_node using allocator   ---   */
    template <
        typename... EmplaceArgs,
        typename FirstChild,
        typename NextSibling,
        typename Allocator = std::allocator<nary_node>,
        typename           = std::enable_if_t<std::is_constructible_v<T, EmplaceArgs...>>>
    explicit nary_node(
        const struct_node<std::tuple<EmplaceArgs...>, FirstChild, NextSibling>& other,
        Allocator&& allocator = Allocator()) :
            nary_node(other, nullptr, nullptr, allocator) {
    }

    /*   ---   Delegated converting constructor from emplacing struct_node using allocator   ---   */
    template <
        typename... EmplaceArgs,
        typename FirstChild,
        typename NextSibling,
        typename Allocator,
        typename = std::enable_if_t<std::is_constructible_v<T, EmplaceArgs...>>>
    explicit nary_node(
        const struct_node<std::tuple<EmplaceArgs...>, FirstChild, NextSibling>& other,
        nary_node* parent,
        nary_node* prev_sibling,
        Allocator&& allocator) :
            node<T, nary_node>(parent, other.get_value()),
            following_size(std::decay_t<decltype(other)>::following_siblings()),
            prev_sibling(prev_sibling),
            next_sibling(
                [&]() {
                    if constexpr (is_empty<NextSibling>) {
                        return nullptr;
                    } else {
                        return allocate(allocator, other.get_next_sibling(), parent, this, allocator).release();
                    }
                }()),
            first_child([&]() {
                if constexpr (is_empty<FirstChild>) {
                    return nullptr;
                } else {
                    return allocate(allocator, other.get_first_child(), this, nullptr, allocator).release();
                }
            }()) {
        this->manage_parent_last_child();
    }

    /*   ---   METHODS   ---   */
    protected:
    // Every constructor must call this in order to initialize last_child for itself or parent
    void manage_parent_last_child() {
        if (this->parent && !this->next_sibling) {
            this->parent->last_child = this;
        }
        if (!this->first_child) {
            this->last_child = nullptr;
        }
    }

    /// Discard this whole subtree and replace it with node
    void replace_with(nary_node* node) {
        assert(node == nullptr || (node->parent == nullptr && node->next_sibling == nullptr));
        if (this->parent) {
            // Either parent's first_child or prev_siblings's next_sibling
            nary_node** back_link = nullptr;
            // Either node or this->next_sibling
            nary_node* link_target = nullptr;
            if (node != nullptr) {
                node->parent         = this->parent;
                node->following_size = this->following_size;
                node->prev_sibling   = this->prev_sibling;
                node->next_sibling   = this->next_sibling;
                back_link            = this->prev_sibling
                    ? &this->prev_sibling->next_sibling
                    : &this->parent->first_child;
                link_target = node;
            } else {
                nary_node* child = this->parent->first_child;
                // get_prev_sibling
                while (child && child != this) {
                    --child->following_size;
                    back_link = &child->next_sibling;
                    child     = child->next_sibling;
                }
                link_target = this->next_sibling;
            }
            *back_link = link_target;
            // Set parent's last_child
            if (this->is_last_child()) {
                this->parent->last_child = link_target;
            } else {
                this->next_sibling->prev_sibling = node ? node : this->prev_sibling;
            }
            this->parent         = nullptr;
            this->following_size = 0u;
            this->prev_sibling   = nullptr;
            this->next_sibling   = nullptr;
        }
    }

    nary_node* prepend_child(nary_node* node) {
        if (node != nullptr) {
            assert(!node->parent);
            assert(!node->prev_sibling);
            assert(!node->next_sibling);
            node->parent       = this;
            node->next_sibling = this->first_child;
            if (this->first_child) {
                node->following_size             = this->first_child->following_size + 1u;
                node->next_sibling->prev_sibling = node;
            } else {
                this->last_child = node;
            }
            this->first_child = node;
        }
        return node;
    }

    nary_node* append_child(nary_node* node) {
        if (node) {
            assert(node->parent == nullptr);
            assert(node->next_sibling == nullptr);
            if (this->first_child) {
                nary_node* current = this->first_child;
                do {
                    current->following_size += 1u;
                    current = current->next_sibling;
                } while (current != nullptr);
            } else {
                this->first_child = node;
            }
            node->parent = this;
            if (this->last_child) {
                this->last_child->next_sibling = node;
                node->prev_sibling             = this->last_child;
            }
            this->last_child = node;
        }
        return node;
    }

    template <typename Node>
    static Node* calculate_child(Node* ptr, std::size_t index) {
        Node* current = ptr->first_child;
        // Trivil last child shortcut
        if (current && index == current->following_size) {
            return ptr->last_child;
        }
        for (std::size_t i = 0; current && i < index; ++i) {
            current = current->next_sibling;
        }
        return current;
    }

    public:
    bool is_last_child() const {
        return this->parent
            ? this->parent->last_child == this
            : false;
    }

    bool is_first_child() const {
        return this->parent
            ? this->parent->first_child == this
            : false;
    }

    const nary_node* get_prev_sibling() const {
        return this->prev_sibling;
    }

    nary_node* get_prev_sibling() {
        return this->prev_sibling;
    }

    const nary_node* get_next_sibling() const {
        return this->next_sibling;
    }

    nary_node* get_next_sibling() {
        return this->next_sibling;
    }

    const nary_node* get_first_child() const {
        return this->first_child;
    }

    nary_node* get_first_child() {
        return this->first_child;
    }

    const nary_node* get_last_child() const {
        return this->last_child;
    }

    nary_node* get_last_child() {
        return this->last_child;
    }

    const nary_node* get_child(std::size_t index) const {
        return calculate_child(this, index);
    }

    nary_node* get_child(std::size_t index) {
        return calculate_child(this, index);
    }

    std::size_t children() const {
        return this->first_child
            ? this->first_child->following_size + 1
            : 0u;
    }

    std::size_t following_siblings() const {
        return this->following_size;
    }

    std::tuple<nary_node*, nary_node*> get_resources() {
        return std::make_tuple(this->first_child, this->next_sibling);
    }

    template <typename Allocator>
    nary_node* assign_child_like(unique_node_ptr<Allocator> child, const nary_node&) {
        assert(child);
        return this->append_child(child.release());
    }

    template <typename Allocator>
    unique_node_ptr<Allocator> allocate_assign_parent(Allocator& allocator, const nary_node& reference_copy) {
        assert(!reference_copy.is_root());
        auto parent = allocate(allocator, reference_copy.get_parent()->get_value());
        parent->append_child(this);
        return std::move(parent);
    }

    /*   ---   Compare egains itself   ---   */
    bool operator==(const nary_node& other) const {
        // Trivial case exclusion
        if (this->children() != other.children()) {
            return false;
        }
        // Test value for inequality
        if (this->value != other.value) {
            return false;
        }
        // Deep comparison (at this point both are either null or something)
        if (this->first_child && *this->first_child != *other.first_child) {
            return false;
        }
        // Deep comparison (at this point both are either null or something)
        if (this->next_sibling && *this->next_sibling != *other.next_sibling) {
            return false;
        }
        // All the possible false cases were tested, then it's true
        return true;
    }

    /*   ---   Compare egains binary_node   ---   */
    bool operator==(const binary_node<T>& other) const {
        // Trivial case exclusion
        if (this->children() != other.children()) {
            return false;
        }
        // Test value for inequality
        if (this->get_value() != other.get_value()) {
            return false;
        }
        // Deep comparison (at this point both are either null or something)
        if (this->has_children() && !this->first_child->operator==(*other.get_first_child())) {
            return false;
        }
        // Deep comparison (at this point both are either null or something)
        if (this->next_sibling && !this->next_sibling->operator==(*other.get_next_sibling())) {
            return false;
        }
        // All the possible false cases were tested, then it's true
        return true;
    }

    /*   ---   Compare against struct_node   ---   */
    template <
        typename ConvertibleT = T,
        typename FirstChild,
        typename NextSibling,
        typename = std::enable_if_t<std::is_convertible_v<ConvertibleT, T>>>
    bool operator==(const struct_node<ConvertibleT, FirstChild, NextSibling>& other) const {
        // One of the subtree has children while the other doesn't
        if (this->children() != other.children()) {
            return false;
        }
        // Test value for inequality
        if (!(this->value == other.get_value())) {
            return false;
        }
        if constexpr (!is_empty<FirstChild>) {
            if constexpr (is_empty_node<FirstChild>) {
                return false;
            } else {
                // Deep comparison (at this point both are either null or something)
                if (*this->get_first_child() != other.get_first_child()) {
                    return false;
                }
            }
        }
        if constexpr (!is_empty<NextSibling>) {
            if constexpr (is_empty_node<NextSibling>) {
                return false;
            } else {
                // Deep comparison (at this point both are either null or something)
                if (*this->get_next_sibling() != other.get_next_sibling()) {
                    return false;
                }
            }
        }
        return true;
    }

    template <typename MatcherNode>
    bool accept(const MatcherNode& matcher) {
        return matcher.match(*this);
    }
};

/*   ---   Expected equality properties  ---   */

// nary_node
template <typename T>
bool operator!=(const nary_node<T>& lhs, const nary_node<T>& rhs) {
    return !lhs.operator==(rhs);
}

// binary_node
template <typename T>
bool operator==(const binary_node<T>& lhs, const nary_node<T>& rhs) {
    return rhs.operator==(lhs);
}

// binary_node
template <typename T>
bool operator!=(const nary_node<T>& lhs, const binary_node<T>& rhs) {
    return !lhs.operator==(rhs);
}

// binary_node
template <typename T>
bool operator!=(const binary_node<T>& lhs, const nary_node<T>& rhs) {
    return !rhs.operator==(lhs);
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
    const nary_node<T>& rhs) {
    return rhs.operator==(lhs);
}

template <
    typename T,
    typename ConvertibleT,
    typename FirstChild,
    typename NextSibling,
    typename = std::enable_if_t<std::is_convertible_v<ConvertibleT, T>>>
bool operator!=(
    const nary_node<T>& lhs,
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
    const nary_node<T>& rhs) {
    return !rhs.operator==(lhs);
}

} // namespace md
