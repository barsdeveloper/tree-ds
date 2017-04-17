#ifndef HE70CB18C_6F01_4598_BF63_4F076505F06A
#define HE70CB18C_6F01_4598_BF63_4F076505F06A

#include <memory>

#include <TreeDS/tree.hpp>

namespace std {

template <typename T, typename Alg, typename Alloc>
struct hash<ds::tree<T, Alg, Alloc>> {
};

} /* namespace std */

namespace ds {

template <typename T>
void swap(tree_base<T>& a, tree_base<T>& b) {
	return a.swap(b);
}

} /* namespace ds */

#endif /* HE70CB18C_6F01_4598_BF63_4F076505F06A */
