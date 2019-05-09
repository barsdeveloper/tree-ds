#pragma once

#include <cassert> // assert()
#include <memory>  // std::unique_ptr, std::allocator_traits
#include <tuple>

#include <TreeDS/utility.hpp>

namespace md {

template <typename Allocator>
using allocator_value_type = typename std::allocator_traits<std::remove_reference_t<Allocator>>::value_type;

template <typename TargetType, typename Allocator, typename = void>
constexpr bool is_allocator_correct_type = false;

template <typename TargetType, typename Allocator>
constexpr bool is_allocator_correct_type<
    TargetType,
    Allocator,
    std::enable_if_t<
        std::is_same_v<allocator_value_type<Allocator>, TargetType>>> = true;

/**
 * @brief Deallocates a previously allocated value using the given allocator.
 *
 * This function contains the logic needed to deallocate a value (represented by a pointer) previously allocated. This
 * does not make any assumption on the allocator (except what is required by the standard:
 * https://en.cppreference.com/w/cpp/named_req/Allocator )
 *
 * @tparam Allocator the type of the allocator
 * @param allocator a reference to an actual Allocator object
 * @param ptr pointer to a previously allocated value
 */
template <typename Allocator>
void deallocate(Allocator& allocator, typename Allocator::value_type* ptr) {
    if (ptr == nullptr) {
        return;
    }
    if constexpr (holds_resources<decltype(*ptr)>) {
        // First deallocate nodes that this node is responsible for
        auto resources_to_deallocate = ptr->get_resources();
        std::apply(
            [&](auto&... pointers) {
                (..., deallocate(allocator, pointers));
            },
            resources_to_deallocate);
    }
    // Then the node itself
    // Call destructor
    std::allocator_traits<Allocator>::destroy(allocator, ptr);
    // Free memory
    std::allocator_traits<Allocator>::deallocate(allocator, ptr, 1);
}

/**
 * @brief Wraps an allocator to easily deallocate a value (calling {@link #operator()(Allocator::value_type*)}).
 *
 * This function is needed to be given to a std::unique_ptr as Deallocator.
 *
 * @tparam Allocator the type of the allocator
 */
template <typename Allocator>
struct deleter {
    Allocator allocator;
    deleter() :
            allocator() {
    }
    deleter(Allocator& allocator) :
            allocator(allocator) {
    }
    void operator()(typename Allocator::value_type* ptr) {
        deallocate(allocator, ptr);
    }
};

template <typename NodeAllocactor>
using unique_node_ptr = std::unique_ptr<typename NodeAllocactor::value_type, deleter<NodeAllocactor>>;

/**
 * @private
 * @brief Allocates a new value using the allocator provided.
 *
 * @tparam Allocator the type of the allocator the type of the allocator
 * @tparam Args types of arguments to be provided to the constructor of allocated type
 * @param allocator the allocator object
 * @param args arguments to be provided to the constructor of allocated type
 * @return std::unique_ptr<typename Allocator::value_type, deleter<Allocator>>
 */
template <
    typename Allocator,
    typename... Args>
std::unique_ptr<typename Allocator::value_type, deleter<Allocator>>
allocate(Allocator& allocator, Args&&... args) {
    // Allocate
    auto* ptr = std::allocator_traits<Allocator>::allocate(allocator, 1);
    // Construct
    std::allocator_traits<Allocator>::construct(allocator, ptr, std::forward<Args>(args)...);
    // Return result
    return std::unique_ptr<typename Allocator::value_type, deleter<Allocator>>(
        ptr,
        {allocator});
}

} // namespace md
