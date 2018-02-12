#ifndef HE70CB18C_6F01_4598_BF63_4F076505F06A
#define HE70CB18C_6F01_4598_BF63_4F076505F06A

#include <memory>

#include <TreeDS/tree_base.hpp>

namespace std {

template <typename T>
void swap(ds::tree_base<T>& a, ds::tree_base<T>& b) {
    a.swap(b);
}

} /* namespace std */

#endif /* HE70CB18C_6F01_4598_BF63_4F076505F06A */
