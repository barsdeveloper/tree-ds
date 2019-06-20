#pragma once

#include <cstddef>     // std::size_t
#include <tuple>       // std::apply()
#include <type_traits> // std::enable_if_t, std::decay_t, std::remove_pointer_t

#include <TreeDS/utility.hpp>

namespace md {

template <typename MainPtr, typename... OtherPtr>
class multiple_node_pointer {

    static_assert((... && is_const_cast_equivalent<MainPtr, OtherPtr>), "The pointer must point to the same type");

    std::tuple<MainPtr, OtherPtr...> pointers;

    public:
    multiple_node_pointer() :
            multiple_node_pointer(nullptr) {
    }

    multiple_node_pointer(MainPtr main_ptr, OtherPtr... pointers) :
            pointers(main_ptr, pointers...) {
    }

    multiple_node_pointer(std::nullptr_t) :
            pointers() {
    }

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
        typename N = std::decay_t<std::remove_pointer_t<MainPtr>>,
        typename   = std::enable_if_t<is_same_template<N, binary_node<void>>>>
    multiple_node_pointer get_left_child() const {
        return do_function([](auto& node) {
            return node.get_left_child();
        });
    }

    template <
        typename N = std::decay_t<std::remove_pointer_t<MainPtr>>,
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

    std::tuple<MainPtr, OtherPtr...>& get_pointers() {
        return this->pointers;
    }

    MainPtr& get_main_pointer() {
        return std::get<0>(this->pointers);
    }

    bool all_valid() const {
        return std::apply(
            [](auto&... ptrs) {
                return (... && ptrs);
            },
            this->pointers);
    }

    bool operator==(multiple_node_pointer other) const {
        return std::get<0>(this->pointers) == std::get<0>(other.pointers);
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

    operator bool() const {
        return std::get<0>(this->pointers) != nullptr;
    }
};

} // namespace md
