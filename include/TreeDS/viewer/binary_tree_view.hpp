#pragma once

#include <TreeDS/tree_base.hpp>
#include <TreeDS/viewer/tree_view.hpp>

namespace md {

template <
    typename T,
    typename Policy    = default_policy,
    typename Allocator = std::allocator<T>>
class binary_tree_view : public tree_view<binary_node<T>, Policy, Allocator> {

    using tree_view<binary_node<T>, Policy, Allocator>::tree_view;

    public:
    DECLARE_TREEDS_TYPES(const binary_node<T>, Policy, Allocator)
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
char* toString(const binary_tree_view<T, Policy, Allocator>& tree) {
    std::stringstream ss;
    ss << tree;
    return qstrdup((std::string("\n") + ss.str()).c_str());
}
} // namespace md
#endif
