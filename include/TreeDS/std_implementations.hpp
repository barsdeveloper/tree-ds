#pragma once

#include <memory>

#include <TreeDS/tree_base.hpp>

namespace std {

template <typename T>
void swap(ds::tree_base<T>& a, ds::tree_base<T>& b) {
    a.swap(b);
}

} /* namespace std */
