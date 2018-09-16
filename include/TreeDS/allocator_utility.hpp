#pragma once

#include <cassert> // assert()
#include <memory>  // std::unique_ptr, std::allocator_traits

namespace ds {

/**
 * @private
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
    assert(ptr != nullptr);
    // recursively delete children
    if (ptr->left) {
        deallocate(allocator, ptr->left);
    }
    if (ptr->right) {
        deallocate(allocator, ptr->right);
    }
    // destroy
    std::allocator_traits<Allocator>::destroy(allocator, ptr);
    // deallocate
    std::allocator_traits<Allocator>::deallocate(allocator, ptr, 1);
}

/**
 * @private
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
    // allocate
    auto* ptr = std::allocator_traits<Allocator>::allocate(allocator, 1);
    // construct
    std::allocator_traits<Allocator>::construct(allocator, ptr, std::forward<Args>(args)...);
    // return result
    return std::unique_ptr<
        typename Allocator::value_type,
        deleter<Allocator>>(ptr, {allocator});
}

} // namespace ds
