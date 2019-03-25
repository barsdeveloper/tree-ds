#pragma once

#include <functional> // std::mem_fn()
#include <tuple>
#include <utility> // std::move(), std::forward()

#include <TreeDS/allocator_utility.hpp>
#include <TreeDS/node/binary_node.hpp>
#include <TreeDS/node/node.hpp>
#include <TreeDS/node/struct_node.hpp>
#include <TreeDS/utility.hpp> // keep_calling()

namespace md {

template <typename T>
class nary_node : public node<T, nary_node<T>> {

    template <typename, typename, bool>
    friend class tree_iterator;

    template <typename, typename, typename, typename>
    friend class tree;

    protected:
    std::size_t following_siblings = 0u;
    nary_node* next_sibling        = nullptr;
    nary_node* first_child         = nullptr;
    nary_node* last_child; // it will be set by manage_parent_last_child() <- every constructor must call this method

    public:
    using node<T, nary_node<T>>::node;

    public:
    // Forward constructor: the arguments are forwarded directly to the constructor of the type T.
    template <typename... Args, CHECK_CONSTRUCTIBLE(T, Args...)>
    explicit nary_node(Args&&... args) :
            node<T, nary_node>(nullptr, std::forward<Args>(args)...) {
        this->manage_parent_last_child();
    }

    // Forward constructor: the arguments are forwarded directly to the constructor of the type T (packed as tuple).
    template <typename... Args, CHECK_CONSTRUCTIBLE(T, Args...)>
    explicit nary_node(const std::tuple<Args...>& args_tuple) :
            node<T, nary_node>(nullptr, args_tuple) {
        this->manage_parent_last_child();
    }

    /*   ---   Move Constructor   ---   */
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
                other.get_first_child() != nullptr
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
        CHECK_CONVERTIBLE(ConvertibleT, T)>
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
        CHECK_CONSTRUCTIBLE(T, EmplaceArgs...)>
    explicit nary_node(
        const struct_node<std::tuple<EmplaceArgs...>, Nodes...>& other,
        Allocator&& allocator = Allocator()) :
            node<T, nary_node>(other.get_value()) {
        this->manage_children(other.get_children(), allocator);
        this->manage_parent_last_child();
    }

    protected:
    // every constructor must call this in order to initialize last_child for itself or parent
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
        // pointer to the considered child (which is itself a pointer to nary_node)
        nary_node* result         = nullptr;
        nary_node** current_child = &result;
        // lambda that assigns one child at time
        auto assign_child = [&](auto node) {
            *current_child                       = node;             // assign child
            (*current_child)->following_siblings = --children_count; // assign siblings count
            (*current_child)->parent             = this;             // set the parent of that child
            if (children_count == 0) {
                this->last_child = *current_child;
            }
            current_child = &(*current_child)->next_sibling; // go to next child
        };
        // lambda that constructs (by calling allocate) a nary_node from a struct_node
        auto process_child = [&](auto& structure_node) {
            static_assert(
                !std::is_same_v<decltype(structure_node.get_value()), detail::empty_node_t>,
                "ds::nary_node does not accept empty nodes");
            assign_child(allocate(allocator, structure_node, allocator).release());
        };
        std::apply(
            // call allocate_child for each element in the tuple other.get_children()
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
            // set parent't first_child or prev_siblings's next_sibling
            if (back_link == nullptr) {
                back_link = this->is_first_child()
                    ? &this->parent->first_child
                    : &calculate_prev_sibling(this)->next_sibling;
            }
            *back_link = link_target;
            // set parent's last_child
            if (this->next_sibling == nullptr) {
                this->parent->last_child = link_target;
            }
            this->next_sibling       = nullptr;
            this->following_siblings = 0u;
            this->parent             = nullptr;
        }
    }

    void prepend_child(nary_node* node) {
        assert(node != nullptr);
        assert(node->parent == nullptr);
        assert(node->next_sibling == nullptr);
        node->parent       = this;
        node->next_sibling = this->first_child;
        this->first_child  = node;
        if (this->last_child == nullptr) {
            this->last_child = node;
        }
    }

    void append_child(nary_node* node) {
        assert(node != nullptr);
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
    }

    template <typename Node>
    static Node* calculate_prev_sibling(Node* ptr) {
        if (ptr->parent && ptr->parent->first_child) {
            const nary_node* first = ptr->parent->first_child;
            if (first == ptr) {
                return nullptr;
            }
            /*
             * const_cast is safe because. All the methods of the nodes (get_parent(), get_whatever(), ...) return
             * pointers to const Node*. This was done in order to simplify the API and, at the same time, prevent users
             * from modifying the subtree structure (without using tree::insert(), tree::erase(), ...). At the same
             * time, the pointers returned by those getter methods refer to modifiable tree node (when the node it was
             * called from, is modifiable itself). In practire either all the tree structure is modifiable or no node
             * can be modified. For this reason, doing a const_cast is safe when you start from a modifiable node and
             * remain inside the starting tree structure.
             */
            return const_cast<Node*>(
                keep_calling(
                    // starting from
                    *first,
                    // keep calling
                    [](const nary_node& node) {
                        return node.next_sibling;
                    },
                    // until
                    [&](const nary_node&, const nary_node& next) {
                        return &next == ptr;
                    },
                    // then return
                    [](const nary_node& prev, const nary_node&) {
                        return &prev;
                    }));
        } else {
            return nullptr;
        }
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

    const nary_node* get_first_child() const {
        return this->first_child;
    }

    const nary_node* get_last_child() const {
        return this->last_child;
    }

    const nary_node* get_prev_sibling() const {
        return nary_node::calculate_prev_sibling(this);
    }

    const nary_node* get_next_sibling() const {
        return this->next_sibling;
    }

    const nary_node* get_prev_sibling_limit(const nary_node& root) const {
        return this->is_root_limit(root) ? nullptr : this->get_prev_sibling();
    }

    const nary_node* get_next_sibling_limit(const nary_node& root) const {
        return this->is_root_limit(root) ? nullptr : this->get_next_sibling();
    }

    std::size_t children() const {
        return this->first_child
            ? this->first_child->get_following_siblings() + 1
            : 0u;
    }

    std::size_t get_following_siblings() const {
        return this->following_siblings;
    }

    const nary_node* get_child(std::size_t index) const {
        const nary_node* const* current = &this->first_child;
        if (*current != nullptr && index == (*current)->following_siblings) {
            return this->last_child;
        }
        for (std::size_t i = 0; i < index && *current != nullptr; ++i) {
            current = &(*current)->next_sibling;
        }
        return *current;
    }

    std::tuple<nary_node*, nary_node*> get_resources() {
        return std::make_tuple(this->first_child, this->next_sibling);
    }

    /*   ---   Compare egains itself   ---   */
    bool operator==(const nary_node& other) const {
        // Trivial case exclusion.
        if ((this->first_child == nullptr) != (other.first_child == nullptr)
            || (this->next_sibling == nullptr) != (other.next_sibling == nullptr)) {
            return false;
        }
        // Test value for inequality.
        if (this->value != other.value) {
            return false;
        }
        // Deep comparison (at this point both are either null or something).
        if (this->first_child && *this->first_child != *other.first_child) {
            return false;
        }
        // Deep comparison (at this point both are either null or something).
        if (this->next_sibling && *this->next_sibling != *other.next_sibling) {
            return false;
        }
        // All the possible false cases were tested, then it's true.
        return true;
    }

    /*   ---   Compare egains binary_node   ---   */
    bool operator==(const binary_node<T>& other) const {
        // Trivial case exclusion.
        if ((this->get_first_child() == nullptr) != (other.get_first_child() == nullptr)) {
            return false;
        }
        // Test value for inequality.
        if (this->get_value() != other.get_value()) {
            return false;
        }
        // Deep comparison (at this point both are either null or something).
        if (this->first_child && !this->first_child->operator==(*other.get_first_child())) {
            return false;
        }
        // Deep comparison (at this point both are either null or something).
        if (this->next_sibling && !this->next_sibling->operator==(*other.get_next_sibling())) {
            return false;
        }
        // All the possible false cases were tested, then it's true.
        return true;
    }

    /*   ---   Compare against struct_node   ---   */
    template <
        typename ConvertibleT = T,
        typename... Nodes,
        CHECK_CONVERTIBLE(ConvertibleT, T)>
    bool operator==(const struct_node<ConvertibleT, Nodes...>& other) const {
        // One of the subtree has children while the other doesn't
        if ((this->first_child == nullptr) != (other.children_count() == 0)) {
            return false;
        }
        // Test value for inequality.
        if (this->value != other.get_value()) {
            return false;
        }
        // pointer to the considered child (which is itself a pointer to const nary_node)
        const nary_node* const* current_child = &this->first_child;
        // lambda that constructs (by calling allocate) a nary_node from a struct_node
        auto compare_child = [&](auto& structure_node) -> bool {
            bool result;
            if constexpr (std::is_same_v<decltype(structure_node.get_value()), detail::empty_node_t>) {
                result = *current_child == nullptr;
            } else { // not empty node
                result = *current_child && **current_child == structure_node;
            }
            current_child = &(*current_child)->next_sibling;
            return result;
        };
        return std::apply(
                   [&](auto&... nodes) {
                       // call compare_child for each element in the tuple other.get_children() and check all are true
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
    CHECK_CONVERTIBLE(ConvertibleT, T)>
bool operator==(
    const struct_node<ConvertibleT, Children...>& lhs,
    const nary_node<T>& rhs) {
    return rhs.operator==(lhs);
}

template <
    typename T,
    typename ConvertibleT,
    typename... Children,
    CHECK_CONVERTIBLE(ConvertibleT, T)>
bool operator!=(
    const nary_node<T>& lhs,
    const struct_node<ConvertibleT, Children...>& rhs) {
    return !lhs.operator==(rhs);
}

template <
    typename T,
    typename ConvertibleT,
    typename... Children,
    CHECK_CONVERTIBLE(ConvertibleT, T)>
bool operator!=(
    const struct_node<ConvertibleT, Children...>& lhs,
    const nary_node<T>& rhs) {
    return !rhs.operator==(lhs);
}

} // namespace md
