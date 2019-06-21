#pragma once

#include <cstddef>     // std::size_t
#include <tuple>       // std::apply()
#include <type_traits> // std::enable_if_t, std::decay_t, std::remove_pointer_t

#include <TreeDS/utility.hpp>

namespace md {

template <typename MasterPtr, typename... SlavePtrs>
class multiple_node_pointer {

    /*   ---   VALIDATION   ---   */
    static_assert(std::is_pointer_v<MasterPtr> && (... && std::is_pointer_v<SlavePtrs>), "Type types must be pointers");
    static_assert((... && is_const_cast_equivalent<MasterPtr, SlavePtrs>), "The pointers must point to the same type");

    /*   ---   ATTRIBUTES   ---   */
    protected:
    std::tuple<MasterPtr, SlavePtrs...> pointers;
    std::array<bool, 1 + sizeof...(SlavePtrs)> assigned_ptrs {false};

    /*   ---   CONSTRUCTORS   ---   */
    public:
    multiple_node_pointer() :
            multiple_node_pointer(nullptr) {
    }

    multiple_node_pointer(const multiple_node_pointer& other) :
            pointers(other.pointers),
            assigned_ptrs {other.assigned_ptrs} {
    }

    multiple_node_pointer(MasterPtr main_ptr, SlavePtrs... pointers) :
            pointers(main_ptr, pointers...) {
    }

    multiple_node_pointer(std::nullptr_t) :
            pointers() {
    }

    multiple_node_pointer& operator=(const multiple_node_pointer& other) {
        this->pointers      = other.pointers;
        this->assigned_ptrs = other.assigned_ptrs;
        return *this;
    }

    /*   ---   METHODS   ---   */
    private:
    template <typename Function>
    multiple_node_pointer do_function(Function&& call) const {
        return std::apply(
            [&](auto&... node_ptr) {
                return multiple_node_pointer((node_ptr != nullptr ? call(*node_ptr) : nullptr)...);
            },
            this->pointers);
    }

    public:
    auto& get_value() const {
        return std::get<0>(this->pointers)->get_value();
    }

    multiple_node_pointer get_parent() const {
        return do_function([](auto& node) {
            return node.get_parent();
        });
    }

    multiple_node_pointer get_prev_sibling() const {
        return do_function([](auto& node) {
            return node.get_prev_sibling();
        });
    }

    multiple_node_pointer get_next_sibling() const {
        return do_function([](auto& node) {
            return node.get_next_sibling();
        });
    }

    template <
        typename N = std::decay_t<std::remove_pointer_t<MasterPtr>>,
        typename   = std::enable_if_t<is_same_template<N, binary_node<void>>>>
    multiple_node_pointer get_left_child() const {
        return do_function([](auto& node) {
            return node.get_left_child();
        });
    }

    template <
        typename N = std::decay_t<std::remove_pointer_t<MasterPtr>>,
        typename   = std::enable_if_t<is_same_template<N, binary_node<void>>>>
    multiple_node_pointer get_right_child() const {
        return do_function([](auto& node) {
            return node.get_right_child();
        });
    }

    multiple_node_pointer get_first_child() const {
        return do_function([](auto& node) {
            return node.get_first_child();
        });
    }

    multiple_node_pointer get_last_child() const {
        return do_function([](auto& node) {
            return node.get_last_child();
        });
    }

    multiple_node_pointer get_child(std::size_t index) const {
        return do_function([=](auto& node) {
            return node.get_child(index);
        });
    }

    std::tuple<MasterPtr, SlavePtrs...>& get_pointers() {
        return this->pointers;
    }

    MasterPtr& get_main_pointer() {
        return std::get<0>(this->pointers);
    }

    bool all_valid() const {
        return std::apply(
            [](auto&... ptrs) {
                return (... && ptrs);
            },
            this->pointers);
    }

    template <typename PtrT>
    void assign_pointer(std::size_t index, PtrT value) {
        this->assigned_ptrs[index] = true;
        apply_at_index(
            [&](auto& node_ptr) {
                node_ptr = value;
            },
            this->pointers,
            index);
    }

    void reset_assignment_flags() {
        std::apply(
            [&](auto&&... flags) {
                (..., (flags = false));
            },
            this->pointers);
    }

    bool is_pointer_assigned(std::size_t index) const {
        return this->assigned_ptrs[index];
    }

    MasterPtr get_master_ptr() const {
        return std::get<0>(this->pointers);
    }

    operator bool() const {
        return std::get<0>(this->pointers) != nullptr;
    }

    const multiple_node_pointer* operator->() const {
        return this;
    }

    multiple_node_pointer* operator->() {
        return this;
    }

    const multiple_node_pointer& operator*() const {
        return *this;
    }

    multiple_node_pointer& operator*() {
        return *this;
    }

    bool operator==(multiple_node_pointer other) const {
        return std::get<0>(this->pointers) == std::get<0>(other.pointers);
    }
};

} // namespace md
