#ifndef H40F7331A_27AB_4B81_A85D_B370D0054AD8
#define H40F7331A_27AB_4B81_A85D_B370D0054AD8

#include "helper.hpp"

namespace ds {

template <typename T>
class node;

class pre_order final {

public:
    constexpr pre_order() = default;
    ~pre_order()          = default;

    template <typename T>
    const node<T>* increment(const node<T>& n) const {
        auto result = n.first_child();
        if (result) {
            return result;
        }
        return cross_bridge_right(n);
    }

    template <typename T>
    const node<T>* decrement(const node<T>& n) const {
        const node<T>* next = n.parent();
        const node<T>* prev = &n;
        /*
		 * The parent is the next node if (REMEMBER: we traverse tree in pre-order and decrement the iterator):
		 *     1) The passed node is root (its parent is nullptr so the previous value is the end of the reverse iterator).
		 *     2) The node is the unique child of its parent
		 */
        if (!next || prev == next->first_child()) {
            return next;
        }
        return descent(*next->left_child(), [](const node<T>& n) {
            return n.last_child();
        });
    }

    template <typename T>
    const node<T>* go_first(const node<T>& root) const {
        return &root;
    }

    template <typename T>
    const node<T>* go_last(const node<T>& root) const {
        return descent(root, [](const node<T>& n) {
            return n.last_child();
        });
    }
};

} /* namespace ds */

#endif /* H40F7331A_27AB_4B81_A85D_B370D0054AD8 */
