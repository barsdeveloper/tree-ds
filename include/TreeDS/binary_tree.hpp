#pragma once

#include <memory> // std::allocator

#include <TreeDS/node/binary_node.hpp>
#include <TreeDS/tree.hpp>

namespace md {

/**
 * @brief A type of tree having nodes {@link binary_node}.
 *
 * @tparam T the type of value hold by this tree
 * @tparam Policy default traversal algorithm
 * @tparam Allocator the allocator used to allocate nodes
 * @relates tree
 */
template <
    typename T,
    typename Policy    = default_policy,
    typename Allocator = std::allocator<T>>
class binary_tree : public tree<binary_node<T>, Policy, Allocator> {
    using super = tree<binary_node<T>, Policy, Allocator>;

    public:
    using tree<binary_node<T>, Policy, Allocator>::tree;
    using tree<binary_node<T>, Policy, Allocator>::operator=;
};

} // namespace md

#if !defined NDEBUG && defined QT_VERSION && QT_VERSION >= 050500
#include <QByteArray> // qstrdup()
#include <sstream>    // std::stringstream
#include <string>

#include <TreeDS/utility.hpp>
namespace md {
template <
    typename T,
    typename Policy,
    typename Allocator,
    typename = std::enable_if<is_printable<T>>>
char* toString(const binary_tree<T, Policy, Allocator>& tree) {
    std::stringstream ss;
    ss << tree;
    return qstrdup((std::string("\n") + ss.str()).c_str());
}
} // namespace md
#endif
