#include <QtTest/QtTest>

#include <TreeDS/tree>
#include <TreeDS/view>

using namespace std;
using namespace md;

class NodeNavigatorTest : public QObject {

    Q_OBJECT

    private slots:
    void binary1();
    void binary2();
    void nary1();
};

void NodeNavigatorTest::binary1() {
    binary_node<int> node {
        n(1)(
            n(2)(
                n(4),
                n(5)(
                    n(8),
                    n(9)(
                        n(12),
                        n(13)(
                            n(16))))),
            n(3)(
                n(6)(
                    n(10),
                    n(11)(
                        n(14)(
                            n(),
                            n(17)),
                        n(15))),
                n(7)))};
    node_navigator<binary_node<int>> nav(&node);

    QCOMPARE(nav.get_highest_left_leaf()->get_value(), 4);
    QCOMPARE(nav.get_highest_right_leaf()->get_value(), 7);
    QCOMPARE(nav.get_deepest_leftmost_leaf()->get_value(), 16);
    QCOMPARE(nav.get_deepest_rightmost_leaf()->get_value(), 17);

    QCOMPARE(nav.get_parent(node), nullptr);
    QCOMPARE(nav.get_prev_sibling(node), nullptr);
    QCOMPARE(nav.get_next_sibling(node), nullptr);
    QCOMPARE(nav.get_first_child(node)->get_value(), 2);
    QCOMPARE(nav.get_last_child(node)->get_value(), 3);
    QCOMPARE(nav.get_left_child(node)->get_value(), 2);
    QCOMPARE(nav.get_right_child(node)->get_value(), 3);

    binary_node<int>& n2 = *nav.get_left_child(node);
    binary_node<int>& n3 = *nav.get_right_child(node);

    QCOMPARE(nav.get_parent(n2), &node);
    QCOMPARE(nav.get_parent(n3), &node);
    QCOMPARE(nav.get_prev_sibling(n2), nullptr);
    QCOMPARE(nav.get_next_sibling(n2), &n3);
    QCOMPARE(nav.get_prev_sibling(n3), &n2);
    QCOMPARE(nav.get_next_sibling(n3), nullptr);
    QCOMPARE(nav.get_first_child(n2)->get_value(), 4);
    QCOMPARE(nav.get_last_child(n2)->get_value(), 5);
    QCOMPARE(nav.get_left_child(n2)->get_value(), 4);
    QCOMPARE(nav.get_right_child(n2)->get_value(), 5);
    QCOMPARE(nav.get_first_child(n3)->get_value(), 6);
    QCOMPARE(nav.get_last_child(n3)->get_value(), 7);
    QCOMPARE(nav.get_left_child(n3)->get_value(), 6);
    QCOMPARE(nav.get_right_child(n3)->get_value(), 7);

    binary_node<int>& n4 = *nav.get_left_child(n2);
    binary_node<int>& n5 = *nav.get_right_child(n2);

    QCOMPARE(nav.get_parent(n4), &n2);
    QCOMPARE(nav.get_parent(n5), &n2);
    QCOMPARE(nav.get_prev_sibling(n4), nullptr);
    QCOMPARE(nav.get_next_sibling(n4), &n5);
    QCOMPARE(nav.get_prev_sibling(n5), &n4);
    QCOMPARE(nav.get_next_sibling(n5), nullptr);
    QCOMPARE(nav.get_first_child(n4), nullptr);
    QCOMPARE(nav.get_last_child(n4), nullptr);
    QCOMPARE(nav.get_left_child(n4), nullptr);
    QCOMPARE(nav.get_right_child(n4), nullptr);
    QCOMPARE(nav.get_first_child(n5)->get_value(), 8);
    QCOMPARE(nav.get_last_child(n5)->get_value(), 9);
    QCOMPARE(nav.get_left_child(n5)->get_value(), 8);
    QCOMPARE(nav.get_right_child(n5)->get_value(), 9);

    binary_node<int>& n6 = *nav.get_left_child(n3);
    binary_node<int>& n7 = *nav.get_right_child(n3);

    QCOMPARE(nav.get_parent(n6), &n3);
    QCOMPARE(nav.get_parent(n7), &n3);
    QCOMPARE(nav.get_prev_sibling(n6), nullptr);
    QCOMPARE(nav.get_next_sibling(n6), &n7);
    QCOMPARE(nav.get_prev_sibling(n7), &n6);
    QCOMPARE(nav.get_next_sibling(n7), nullptr);
    QCOMPARE(nav.get_first_child(n6)->get_value(), 10);
    QCOMPARE(nav.get_last_child(n6)->get_value(), 11);
    QCOMPARE(nav.get_left_child(n6)->get_value(), 10);
    QCOMPARE(nav.get_right_child(n6)->get_value(), 11);
    QCOMPARE(nav.get_first_child(n7), nullptr);
    QCOMPARE(nav.get_last_child(n7), nullptr);
    QCOMPARE(nav.get_left_child(n7), nullptr);
    QCOMPARE(nav.get_right_child(n7), nullptr);

    QCOMPARE(nav.get_same_row_leftmost(n5), &n4);
    QCOMPARE(nav.get_same_row_rightmost(n5), &n7);

    binary_node<int>& n12 = *nav.get_left_child(*nav.get_right_child(n5));

    QCOMPARE(nav.get_same_row_leftmost(n12)->get_value(), 12);
    QCOMPARE(nav.get_same_row_rightmost(n12)->get_value(), 15);
}

void NodeNavigatorTest::binary2() {
    const binary_node<int> node {
        n(1)(
            n(),
            n(2)(
                n(3)(
                    n(5)(
                        n(9),
                        n(10)),
                    n(6)),
                n(4)(
                    n(7),
                    n(8)(
                        n(11),
                        n(12)(
                            n(),
                            n(13)(
                                n(14)))))))};
    using nav_t = node_navigator<const binary_node<int>>;
    nav_t nav(&node);

    const binary_node<int>& n2  = *nav.get_right_child(node);
    const binary_node<int>& n3  = *nav.get_left_child(n2);
    const binary_node<int>& n6  = *nav.get_right_child(n3);
    const binary_node<int>& n4  = *nav.get_right_child(n2);
    const binary_node<int>& n5  = *nav.get_left_child(n3);
    const binary_node<int>& n7  = *nav.get_left_child(n4);
    const binary_node<int>& n8  = *nav.get_right_child(n4);
    const binary_node<int>& n9  = *nav.get_left_child(n5);
    const binary_node<int>& n10 = *nav.get_right_child(n5);
    const binary_node<int>& n11 = *nav.get_left_child(n8);
    const binary_node<int>& n12 = *nav.get_right_child(n8);
    const binary_node<int>& n13 = *nav.get_right_child(n12);
    const binary_node<int>& n14 = *nav.get_left_child(n13);

    QCOMPARE(nav_t(&node).get_left_branch(n8), &n7);
    QCOMPARE(nav_t(&n2).get_left_branch(n8), &n7);
    QCOMPARE(nav_t(&n4).get_left_branch(n8), &n7);
    QCOMPARE(nav_t(&n8).get_left_branch(n8), nullptr);
    QCOMPARE(nav_t(&node).get_left_branch(n7), &n6);
    QCOMPARE(nav_t(&n2).get_left_branch(n7), &n6);
    QCOMPARE(nav_t(&n4).get_left_branch(n7), nullptr);
    QCOMPARE(nav_t(&n7).get_left_branch(n7), nullptr);
    QCOMPARE(nav_t(&node).get_left_branch(n12), &n11);
    QCOMPARE(nav_t(&n2).get_left_branch(n12), &n11);
    QCOMPARE(nav_t(&n4).get_left_branch(n12), &n11);
    QCOMPARE(nav_t(&n8).get_left_branch(n12), &n11);
    QCOMPARE(nav_t(&n12).get_left_branch(n12), nullptr);
    QCOMPARE(nav_t(&node).get_left_branch(n11), &n10);
    QCOMPARE(nav_t(&n2).get_left_branch(n11), &n10);
    QCOMPARE(nav_t(&n4).get_left_branch(n11), nullptr);
    QCOMPARE(nav_t(&node).get_left_branch(n13), nullptr);
    QCOMPARE(nav_t(&n8).get_left_branch(n13), nullptr);
    QCOMPARE(nav_t(&node).get_left_branch(n14), nullptr);
    QCOMPARE(nav_t(&node).get_left_branch(n9), nullptr);
    QCOMPARE(nav_t(&node).get_left_branch(n5), nullptr);
    QCOMPARE(nav_t(&node).get_left_branch(n3), nullptr);
    QCOMPARE(nav_t(&node).get_left_branch(node), nullptr);

    QCOMPARE(nav_t(&node).get_right_branch(node), nullptr);
    QCOMPARE(nav_t(&node).get_right_branch(n2), nullptr);
    QCOMPARE(nav_t(&n2).get_right_branch(n2), nullptr);
    QCOMPARE(nav_t(&node).get_right_branch(n3), &n4);
    QCOMPARE(nav_t(&n2).get_right_branch(n3), &n4);
    QCOMPARE(nav_t(&n3).get_right_branch(n3), nullptr);
    QCOMPARE(nav_t(&node).get_right_branch(n5), &n6);
    QCOMPARE(nav_t(&n2).get_right_branch(n5), &n6);
    QCOMPARE(nav_t(&n3).get_right_branch(n5), &n6);
    QCOMPARE(nav_t(&n5).get_right_branch(n5), nullptr);
    QCOMPARE(nav_t(&node).get_right_branch(n6), &n7);
    QCOMPARE(nav_t(&n2).get_right_branch(n6), &n7);
    QCOMPARE(nav_t(&n3).get_right_branch(n6), nullptr);
    QCOMPARE(nav_t(&node).get_right_branch(n10), &n11);
    QCOMPARE(nav_t(&n2).get_right_branch(n10), &n11);
    QCOMPARE(nav_t(&n3).get_right_branch(n10), nullptr);
    QCOMPARE(nav_t(&node).get_right_branch(n11), &n12);
    QCOMPARE(nav_t(&n2).get_right_branch(n11), &n12);
    QCOMPARE(nav_t(&n4).get_right_branch(n11), &n12);
    QCOMPARE(nav_t(&n8).get_right_branch(n11), &n12);
    QCOMPARE(nav_t(&n11).get_right_branch(n11), nullptr);

    QCOMPARE(nav_t(&node).get_same_row_leftmost(node), &node);
    QCOMPARE(nav_t(&node).get_same_row_leftmost(n2), &n2);
    QCOMPARE(nav_t(&n2).get_same_row_leftmost(n2), &n2);
    QCOMPARE(nav_t(&node).get_same_row_leftmost(n3), &n3);
    QCOMPARE(nav_t(&n2).get_same_row_leftmost(n3), &n3);
    QCOMPARE(nav_t(&n3).get_same_row_leftmost(n3), &n3);
    QCOMPARE(nav_t(&node).get_same_row_leftmost(n4), &n3);
    QCOMPARE(nav_t(&n2).get_same_row_leftmost(n4), &n3);
    QCOMPARE(nav_t(&n4).get_same_row_leftmost(n4), &n4);
    QCOMPARE(nav_t(&node).get_same_row_leftmost(n7), &n5);
    QCOMPARE(nav_t(&n2).get_same_row_leftmost(n7), &n5);
    QCOMPARE(nav_t(&n4).get_same_row_leftmost(n7), &n7);
    QCOMPARE(nav_t(&n7).get_same_row_leftmost(n7), &n7);
    QCOMPARE(nav_t(&node).get_same_row_leftmost(n8), &n5);
    QCOMPARE(nav_t(&n2).get_same_row_leftmost(n8), &n5);
    QCOMPARE(nav_t(&n4).get_same_row_leftmost(n8), &n7);
    QCOMPARE(nav_t(&n8).get_same_row_leftmost(n8), &n8);
    QCOMPARE(nav_t(&node).get_same_row_leftmost(n12), &n9);
    QCOMPARE(nav_t(&n2).get_same_row_leftmost(n12), &n9);
    QCOMPARE(nav_t(&n4).get_same_row_leftmost(n12), &n11);
    QCOMPARE(nav_t(&n8).get_same_row_leftmost(n12), &n11);
    QCOMPARE(nav_t(&n12).get_same_row_leftmost(n12), &n12);

    QCOMPARE(nav_t(&node).get_same_row_rightmost(node), &node);
    QCOMPARE(nav_t(&node).get_same_row_rightmost(n9), &n12);
    QCOMPARE(nav_t(&n2).get_same_row_rightmost(n9), &n12);
    QCOMPARE(nav_t(&n3).get_same_row_rightmost(n9), &n10);
    QCOMPARE(nav_t(&n5).get_same_row_rightmost(n9), &n10);
    QCOMPARE(nav_t(&n9).get_same_row_rightmost(n9), &n9);
    QCOMPARE(nav_t(&node).get_same_row_rightmost(n10), &n12);
    QCOMPARE(nav_t(&n2).get_same_row_rightmost(n10), &n12);
    QCOMPARE(nav_t(&n3).get_same_row_rightmost(n10), &n10);
    QCOMPARE(nav_t(&n5).get_same_row_rightmost(n10), &n10);
    QCOMPARE(nav_t(&n10).get_same_row_rightmost(n10), &n10);
    QCOMPARE(nav_t(&node).get_same_row_rightmost(n11), &n12);
    QCOMPARE(nav_t(&n2).get_same_row_rightmost(n11), &n12);
    QCOMPARE(nav_t(&n4).get_same_row_rightmost(n11), &n12);
    QCOMPARE(nav_t(&n8).get_same_row_rightmost(n11), &n12);
    QCOMPARE(nav_t(&n11).get_same_row_rightmost(n11), &n11);
    QCOMPARE(nav_t(&node).get_same_row_rightmost(n14), &n14);
    QCOMPARE(nav_t(&n2).get_same_row_rightmost(n14), &n14);
    QCOMPARE(nav_t(&n4).get_same_row_rightmost(n14), &n14);
    QCOMPARE(nav_t(&n8).get_same_row_rightmost(n14), &n14);
    QCOMPARE(nav_t(&n12).get_same_row_rightmost(n14), &n14);
    QCOMPARE(nav_t(&n13).get_same_row_rightmost(n14), &n14);
    QCOMPARE(nav_t(&n14).get_same_row_rightmost(n14), &n14);
    QCOMPARE(nav_t(&node).get_same_row_rightmost(n13), &n13);
    QCOMPARE(nav_t(&n2).get_same_row_rightmost(n13), &n13);
    QCOMPARE(nav_t(&n4).get_same_row_rightmost(n13), &n13);
    QCOMPARE(nav_t(&n8).get_same_row_rightmost(n13), &n13);
    QCOMPARE(nav_t(&n12).get_same_row_rightmost(n13), &n13);
    QCOMPARE(nav_t(&n13).get_same_row_rightmost(n13), &n13);

    QCOMPARE(nav_t(&node).get_deepest_rightmost_leaf(), &n14);
    QCOMPARE(nav_t(&n3).get_deepest_rightmost_leaf(), &n10);
}

void NodeNavigatorTest::nary1() {

    const nary_node<int> node {
        n(1)(
            n(2)(
                n(5),
                n(6)(
                    n(12)(
                        n(17),
                        n(18)(
                            n(23))),
                    n(13)(
                        n(19))),
                n(7)),
            n(3)(
                n(8)(
                    n(14)),
                n(9)),
            n(4)(
                n(10),
                n(11)(
                    n(15)(
                        n(20),
                        n(21)(
                            n(24)(
                                n(26)(
                                    n(27))),
                            n(25)),
                        n(22)),
                    n(16))))};
    using nav_t = node_navigator<const nary_node<int>>;
    nav_t nav(&node);

    const nary_node<int>& n2  = *nav.get_child(node, 0);
    const nary_node<int>& n3  = *nav.get_child(node, 1);
    const nary_node<int>& n4  = *nav.get_child(node, 2);
    const nary_node<int>& n6  = *nav.get_child(n2, 1);
    const nary_node<int>& n9  = *nav.get_child(n3, 1);
    const nary_node<int>& n10 = *nav.get_child(n4, 0);
    const nary_node<int>& n11 = *nav.get_child(n4, 1);
    const nary_node<int>& n12 = *nav.get_child(n6, 0);
    const nary_node<int>& n13 = *nav.get_child(n6, 1);
    const nary_node<int>& n14
        = *nav.get_child(
            *nav.get_child(n3, 0), // 8
            0);                    // 14
    const nary_node<int>& n15 = *nav.get_child(n11, 0);
    const nary_node<int>& n16 = *nav.get_child(n11, 1);
    const nary_node<int>& n18 = *nav.get_child(n12, 1);
    const nary_node<int>& n19 = *nav.get_child(n13, 0);
    const nary_node<int>& n20 = *nav.get_child(n15, 0);
    const nary_node<int>& n23 = *nav.get_child(n18, 0);
    const nary_node<int>& n24
        = *nav.get_child(
            *nav.get_child(n15, 1), // 21
            0);                     // 24
    const nary_node<int>& n25
        = *nav.get_child(
            *nav.get_child(n15, 1), // n21
            1);                     // n25
    const nary_node<int>& n26 = *nav.get_child(n24, 0);
    const nary_node<int>& n27 = *nav.get_child(n26, 0);

    QCOMPARE(nav_t(&node).get_left_branch(node), nullptr);
    QCOMPARE(nav_t(&node).get_left_branch(n10), &n9);
    QCOMPARE(nav_t(&n4).get_left_branch(n10), nullptr);
    QCOMPARE(nav_t(&n10).get_left_branch(n10), nullptr);
    QCOMPARE(nav_t(&node).get_left_branch(n12), nullptr);
    QCOMPARE(nav_t(&n2).get_left_branch(n12), nullptr);
    QCOMPARE(nav_t(&n12).get_left_branch(n12), nullptr);
    QCOMPARE(nav_t(&node).get_left_branch(n15), &n14);
    QCOMPARE(nav_t(&n4).get_left_branch(n15), nullptr);
    QCOMPARE(nav_t(&n11).get_left_branch(n15), nullptr);
    QCOMPARE(nav_t(&node).get_left_branch(n20), &n19);
    QCOMPARE(nav_t(&n4).get_left_branch(n20), nullptr);
    QCOMPARE(nav_t(&node).get_left_branch(n23), nullptr);
    QCOMPARE(nav_t(&node).get_left_branch(n24), &n23);
    QCOMPARE(nav_t(&node).get_left_branch(n26), nullptr);
    QCOMPARE(nav_t(&node).get_left_branch(n27), nullptr);

    QCOMPARE(nav_t(&node).get_same_row_rightmost(node), &node);
    QCOMPARE(nav_t(&node).get_same_row_rightmost(n2), &n4);
    QCOMPARE(nav_t(&n2).get_same_row_rightmost(n2), &n2);
    QCOMPARE(nav_t(&node).get_same_row_rightmost(n4), &n4);
    QCOMPARE(nav_t(&n4).get_same_row_rightmost(n4), &n4);
    QCOMPARE(nav_t(&node).get_same_row_rightmost(n9), &n11);
    QCOMPARE(nav_t(&n3).get_same_row_rightmost(n9), &n9);
    QCOMPARE(nav_t(&n9).get_same_row_rightmost(n9), &n9);
    QCOMPARE(nav_t(&node).get_same_row_rightmost(n12), &n16);
    QCOMPARE(nav_t(&n2).get_same_row_rightmost(n12), &n13);
    QCOMPARE(nav_t(&n6).get_same_row_rightmost(n12), &n13);
    QCOMPARE(nav_t(&n12).get_same_row_rightmost(n12), &n12);
    QCOMPARE(nav_t(&node).get_same_row_rightmost(n23), &n25);
    QCOMPARE(nav_t(&n2).get_same_row_rightmost(n23), &n23);
    QCOMPARE(nav_t(&n6).get_same_row_rightmost(n23), &n23);
    QCOMPARE(nav_t(&n6).get_same_row_rightmost(n23), &n23);
    QCOMPARE(nav_t(&n12).get_same_row_rightmost(n23), &n23);
    QCOMPARE(nav_t(&n18).get_same_row_rightmost(n23), &n23);
    QCOMPARE(nav_t(&n23).get_same_row_rightmost(n23), &n23);
    QCOMPARE(nav_t(&node).get_same_row_rightmost(n27), &n27);
    QCOMPARE(nav_t(&n4).get_same_row_rightmost(n27), &n27);
    QCOMPARE(nav_t(&n11).get_same_row_rightmost(n27), &n27);
    QCOMPARE(nav_t(&n15).get_same_row_rightmost(n27), &n27);
    QCOMPARE(nav_t(&n24).get_same_row_rightmost(n27), &n27);
    QCOMPARE(nav_t(&n26).get_same_row_rightmost(n27), &n27);
    QCOMPARE(nav_t(&n27).get_same_row_rightmost(n27), &n27);

    QCOMPARE(nav_t(&node).get_deepest_rightmost_leaf(), &n27);
}

QTEST_MAIN(NodeNavigatorTest);
#include "NodeNavigatorTest.moc"
