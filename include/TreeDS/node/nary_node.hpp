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
    template <typename, typename, bool>
    friend class tree_iterator;

    template <typename, typename, typename>
    friend class tree;

    /*   ---   ATTRIBUTES   ---   */
    protected:
    std::size_t following_siblings = 0u;
    nary_node* next_sibling        = nullptr;
    nary_node* first_child         = nullptr;
    nary_node* last_child; // It will be set by manage_parent_last_child() <- every constructor must call this method

    /*   ---   CONSTRUCTORS   ---   */
    public:
    using node<T, nary_node<T>>::node;

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
            following_siblings(other.following_siblings),
            next_sibling(other.next_sibling),
            first_child(other.first_child),
            last_child(other.last_child) {
        other.move_resources_to(*this);
    }

    /*   ---   Copy constructor using allocator   ---   */
    template <typename Allocator>
    explicit nary_node(const nary_node& other, nary_node* parent, Allocator&& allocator = Allocator()) :
            node<T, nary_node<T>>(parent, other.value),
            following_siblings(other.following_siblings),
            next_sibling(
                other.next_sibling != nullptr
                    ? allocate(allocator, *other.next_sibling, this->parent, allocator).release()
                    : nullptr),
            first_child(
                other.first_child != nullptr
                    ? allocate(allocator, *other.first_child, this, allocator).release()
                    : nullptr) {
        this->manage_parent_last_child();
    }

    template <typename Allocator>
    explicit nary_node(const nary_node& other, Allocator&& allocator = Allocator()) :
            nary_node(other, nullptr, allocator) {
    }

    /*   ---   Conversion constructor from binary_node using allocator   ---   */
    template <typename Allocator = std::allocator<nary_node>>
    explicit nary_node(const binary_node<T>& other, nary_node* parent, Allocator&& allocator = Allocator()) :
            node<T, nary_node>(parent, other.get_value()),
            following_siblings(other.get_following_siblings()),
            next_sibling(
                other.get_next_sibling()
                    ? allocate(allocator, *other.get_next_sibling(), this->parent, allocator).release()
                    : nullptr),
            first_child(
                other.has_children()
                    ? allocate(allocator, *other.get_first_child(), this, allocator).release()
                    : nullptr) {
        this->manage_parent_last_child();
    }

    /*   ---   Conversion constructor from binary_node using allocator   ---   */
    template <typename Allocator = std::allocator<nary_node>>
    explicit nary_node(const binary_node<T>& other, Allocator&& allocator = Allocator()) :
            nary_node(other, nullptr, allocator) {
    }

    /*   ---   Construct from struct_node using allocator   ---   */
    template <
        typename ConvertibleT,
        typename... Nodes,
        typename Allocator = std::allocator<nary_node>,
        typename           = std::enable_if_t<std::is_convertible_v<ConvertibleT, T>>>
    explicit nary_node(
        const struct_node<ConvertibleT, Nodes...>& other,
        Allocator&& allocator = std::allocator<nary_node>()) :
            node<T, nary_node>(other.get_value()) {
        this->manage_children(other.get_children(), allocator);
        this->manage_parent_last_child();
    }

    /*   ---   Construct from emplacing struct_node using allocator   ---   */
    template <
        typename... EmplaceArgs,
        typename... Nodes,
        typename Allocator = std::allocator<nary_node>,
        typename           = std::enable_if_t<std::is_constructible_v<T, EmplaceArgs...>>>
    explicit nary_node(
        const struct_node<std::tuple<EmplaceArgs...>, Nodes...>& other,
        Allocator&& allocator = Allocator()) :
            node<T, nary_node>(other.get_value()) {
        this->manage_children(other.get_children(), allocator);
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

    template <typename Allocator, typename... Nodes>
    void manage_children(const std::tuple<Nodes...>& nodes, Allocator& allocator) {
        std::size_t children_count = sizeof...(Nodes);
        // Pointer to the considered child (which is itself a pointer to nary_node)
        nary_node* result         = nullptr;
        nary_node** current_child = &result;
        // Lambda that assigns one child at time
        auto assign_child = [&](auto node) {
            *current_child                       = node;             // Assign child
            (*current_child)->following_siblings = --children_count; // Assign siblings count
            (*current_child)->parent             = this;             // Set the parent of that child
            if (children_count == 0) {
                this->last_child = *current_child;
            }
            current_child = &(*current_child)->next_sibling; // Go to next child
        };
        // Lambda that constructs (by calling allocate) a nary_node from a struct_node
        auto process_child = [&](auto& structure_node) {
            static_assert(
                !std::is_same_v<decltype(structure_node.get_value()), detail::empty_t>,
                "ds::nary_node does not accept empty nodes");
            assign_child(allocate(allocator, structure_node, allocator).release());
        };
        std::apply(
            // Call allocate_child for each element in the tuple other.get_children()
            [&](auto&... nodes) {
                (..., process_child(nodes));
            },
            nodes);
        this->first_child = result;
    }

    /// Move the resources hold by this node to another node
    void move_resources_to(nary_node& node) {
        nary_node* current = this->first_child;
        nary_node* last    = nullptr;
        while (current) {
            last            = current;
            current->parent = &node;
            current         = current->next_sibling;
        }
        if (last != nullptr) {
            last->parent->last_child = last;
        }
        this->parent       = nullptr;
        this->first_child  = nullptr;
        this->last_child   = nullptr;
        this->next_sibling = nullptr;
    }

    /// Discard this whole subtree and replace it with node
    void replace_with(nary_node* node) {
        assert(node == nullptr || (node->parent == nullptr && node->next_sibling == nullptr));
        if (this->parent != nullptr) {
            nary_node** back_link  = nullptr;
            nary_node* link_target = nullptr;
            if (node != nullptr) {
                node->parent             = this->parent;
                node->next_sibling       = this->next_sibling;
                node->following_siblings = this->following_siblings;
                link_target              = node;
            } else {
                nary_node* child = this->parent->first_child;
                // get_prev_sibling
                while (child && child != this) {
                    --child->following_siblings;
                    back_link = &child->next_sibling;
                    child     = child->next_sibling;
                }
                link_target = this->next_sibling;
            }
            // Set parent't first_child or prev_siblings's next_sibling
            if (back_link == nullptr) {
                back_link = this->is_first_child()
                    ? &this->parent->first_child
                    : &calculate_prev_sibling(this)->next_sibling;
            }
            *back_link = link_target;
            // Set parent's last_child
            if (this->next_sibling == nullptr) {
                this->parent->last_child = link_target;
            }
            this->next_sibling       = nullptr;
            this->following_siblings = 0u;
            this->parent             = nullptr;
        }
    }

    nary_node* prepend_child(nary_node* node) {
        if (node != nullptr) {
            assert(node->parent == nullptr);
            assert(node->next_sibling == nullptr);
            node->parent       = this;
            node->next_sibling = this->first_child;
            this->first_child  = node;
            if (this->last_child == nullptr) {
                this->last_child = node;
            }
            return this->first_child;
        }
        return nullptr;
    }

    nary_node* append_child(nary_node* node) {
        if (node != nullptr) {
            assert(node->parent == nullptr);
            assert(node->next_sibling == nullptr);
            node->parent = this;
            if (this->last_child != nullptr) {
                this->last_child->next_sibling = node;
            }
            this->last_child   = node;
            nary_node* current = this->first_child;
            do {
                current->following_siblings += 1u;
                current = current->next_sibling;
            } while (current != nullptr);
            return this->last_child;
        }
        return nullptr;
    }

    template <typename Node>
    static Node* calculate_prev_sibling(Node* ptr) {
        if (ptr->parent && ptr->parent->first_child) {
            nary_node* first = ptr->parent->first_child;
            if (first == ptr) {
                return nullptr;
            }
            return keep_calling(
                // From
                *first,
                // Keep calling
                [](nary_node& node) {
                    return node.next_sibling;
                },
                // Until
                [&](nary_node&, nary_node& next) {
                    return &next == ptr;
                },
                // Then return
                [](nary_node& prev, nary_node&) {
                    return &prev;
                });
        } else {
            return nullptr;
        }
    }

    template <typename Node>
    static Node* calculate_child(Node* ptr, std::size_t index) {
        Node* current = ptr->first_child;
        if (current != nullptr && index == current->following_siblings) {
            return ptr->last_child;
        }
        for (std::size_t i = 0; i < index && current != nullptr; ++i) {
            current = current->next_sibling;
        }
        return current;
    }

    template <typename Allocator>
    nary_node* shallow_copy_assign_child(const nary_node& child, Allocator&& allocator) {
        assert(!child.is_root());
        this->append_child(allocate(allocator, child->get_value()).release());
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
        return nary_node::calculate_prev_sibling(this);
    }

    nary_node* get_prev_sibling() {
        return nary_node::calculate_prev_sibling(this);
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
            ? this->first_child->get_following_siblings() + 1
            : 0u;
    }

    std::size_t get_following_siblings() const {
        return this->following_siblings;
    }
    std::tuple<nary_node*, nary_node*> get_resources() {
        return std::make_tuple(this->first_child, this->next_sibling);
    }

    /*   ---   Compare egains itself   ---   */
    bool operator==(const nary_node& other) const {
        // Trivial case exclusion
        if ((this->first_child == nullptr) != (other.first_child == nullptr)
            || (this->next_sibling == nullptr) != (other.next_sibling == nullptr)) {
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
        if (this->has_children() != other.has_children()) {
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
        typename... Nodes,
        typename = std::enable_if_t<std::is_convertible_v<ConvertibleT, T>>>
    bool operator==(const struct_node<ConvertibleT, Nodes...>& other) const {
        // One of the subtree has children while the other doesn't
        if ((this->first_child == nullptr) != (other.children_count() == 0)) {
            return false;
        }
        // Test value for inequality
        if (this->value != other.get_value()) {
            return false;
        }
        // Pointer to the considered child (which is itself a pointer to const nary_node)
        const nary_node* const* current_child = &this->first_child;
        // Lambda that constructs (by calling allocate) a nary_node from a struct_node
        auto compare_child = [&](auto& structure_node) -> bool {
            bool result;
            if constexpr (std::is_same_v<decltype(structure_node.get_value()), detail::empty_t>) {
                result = *current_child == nullptr;
            } else { // Not empty node
                result = *current_child && **current_child == structure_node;
            }
            current_child = &(*current_child)->next_sibling;
            return result;
        };
        return std::apply(
                   [&](auto&... nodes) {
                       // Call compare_child for each element in the tuple other.get_children() and check all are true
                       return (compare_child(nodes) && ...);
                   },
                   other.get_children())
            && *current_child == nullptr;
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
    typename... Children,
    typename = std::enable_if_t<std::is_convertible_v<ConvertibleT, T>>>
bool operator==(
    const struct_node<ConvertibleT, Children...>& lhs,
    const nary_node<T>& rhs) {
    return rhs.operator==(lhs);
}

template <
    typename T,
    typename ConvertibleT,
    typename... Children,
    typename = std::enable_if_t<std::is_convertible_v<ConvertibleT, T>>>
bool operator!=(
    const nary_node<T>& lhs,
    const struct_node<ConvertibleT, Children...>& rhs) {
    return !lhs.operator==(rhs);
}

template <
    typename T,
    typename ConvertibleT,
    typename... Children,
    typename = std::enable_if_t<std::is_convertible_v<ConvertibleT, T>>>
bool operator!=(
    const struct_node<ConvertibleT, Children...>& lhs,
    const nary_node<T>& rhs) {
    return !rhs.operator==(lhs);
}

} // namespace md
