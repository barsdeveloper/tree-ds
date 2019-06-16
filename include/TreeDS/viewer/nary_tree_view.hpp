#pragma once

#include <memory> // std::allocator

#include <TreeDS/tree_base.hpp>
#include <TreeDS/viewer/binary_tree_view.hpp>
#include <TreeDS/viewer/tree_view.hpp>

namespace md {

template <
    typename T,
    typename Policy    = default_policy,
    typename Allocator = std::allocator<T>>
class nary_tree_view : public tree_view<const nary_node<T>, Policy, Allocator> {

    using tree_view<const nary_node<T>, Policy, Allocator>::tree_view;

    public:
    DECLARE_TREEDS_TYPES(const nary_node<T>, Policy, Allocator)

    // Import the overloads of the operator== into the current class (would be shadowed otherwise)
    using tree_view<const nary_node<T>, Policy, Allocator>::operator==;

    template <typename OtherPolicy>
    bool operator==(const binary_tree_view<T, OtherPolicy, Allocator>& other) const {
        // Test if different size_value
        if (this->size() != other.size()
            || this->arity() != other.arity()) {
            return false;
        }
        // At the end is either null (both) or same as the other
        return this->root == nullptr || *this->root == *other.get_raw_root();
    }
};

template <
    typename T,
    typename Policy1,
    typename Policy2,
    typename Allocator>
bool operator==(
    const binary_tree_view<T, Policy1, Allocator>& lhs,
    const nary_tree_view<T, Policy2, Allocator>& rhs) {
    return rhs.operator==(lhs);
}

template <
    typename T,
    typename Policy1,
    typename Policy2,
    typename Allocator>
bool operator!=(
    const nary_tree_view<T, Policy1, Allocator>& lhs,
    const binary_tree_view<T, Policy2, Allocator>& rhs) {
    return !lhs.operator==(rhs);
}

template <
    typename T,
    typename Policy1,
    typename Policy2,
    typename Allocator>
bool operator!=(
    const binary_tree_view<T, Policy1, Allocator>& lhs,
    const nary_tree_view<T, Policy2, Allocator>& rhs) {
    return !rhs.operator==(lhs);
}

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
char* toString(const nary_tree_view<T, Policy, Allocator>& tree) {
    std::stringstream ss;
    ss << tree;
    return qstrdup((std::string("\n") + ss.str()).c_str());
}
} // namespace md
#endif
