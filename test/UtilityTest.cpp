#include <QtTest/QtTest>

#include <TreeDS/tree>

#include "Types.hpp"

using namespace std;
using namespace md;
using namespace md::detail;

class UtilityTest : public QObject {

    Q_OBJECT

    private slots:
    void singleNode();
    void oneChild();
    void multipleChildren();
    void advancedBinary();
    void advancedNary();
    void isTagOfPolicyTest();
};

void UtilityTest::singleNode() {
    auto single = n(123);
    nary_node<int> nary(single);
    binary_node<int> binary(single);

    QCOMPARE(left_branch_node(nary, nary), nullptr);
    QCOMPARE(left_branch_node(binary, binary), nullptr);

    QCOMPARE(right_branch_node(nary, nary), nullptr);
    QCOMPARE(right_branch_node(binary, binary), nullptr);

    QCOMPARE(same_row_leftmost(nary, nary), &nary);
    QCOMPARE(same_row_leftmost(binary, binary), &binary);

    QCOMPARE(same_row_rightmost(nary, nary), &nary);
    QCOMPARE(same_row_rightmost(binary, binary), &binary);

    QCOMPARE(deepest_rightmost_child(nary), &nary);
    QCOMPARE(deepest_rightmost_child(binary), &binary);
}

void UtilityTest::oneChild() {
    auto leftChild  = n(234)(n(345));
    auto rightChild = n(456)(n(), n(567));
    nary_node<int> nary(leftChild);
    binary_node<int> binaryL(leftChild);
    binary_node<int> binaryR(rightChild);

    QCOMPARE(left_branch_node(nary, nary), nullptr);
    QCOMPARE(left_branch_node(binaryL, binaryL), nullptr);
    QCOMPARE(left_branch_node(binaryR, binaryR), nullptr);
    QCOMPARE(left_branch_node(*nary.get_first_child(), nary), nullptr);
    QCOMPARE(left_branch_node(*binaryL.get_first_child(), binaryL), nullptr);
    QCOMPARE(left_branch_node(*binaryR.get_first_child(), binaryR), nullptr);

    QCOMPARE(right_branch_node(nary, nary), nullptr);
    QCOMPARE(right_branch_node(binaryL, binaryL), nullptr);
    QCOMPARE(right_branch_node(binaryR, binaryR), nullptr);
    QCOMPARE(right_branch_node(*nary.get_first_child(), nary), nullptr);
    QCOMPARE(right_branch_node(*binaryL.get_first_child(), binaryL), nullptr);
    QCOMPARE(right_branch_node(*binaryR.get_first_child(), binaryR), nullptr);

    QCOMPARE(same_row_leftmost(nary, nary), &nary);
    QCOMPARE(same_row_leftmost(binaryL, binaryL), &binaryL);
    QCOMPARE(same_row_leftmost(binaryR, binaryR), &binaryR);
    QCOMPARE(same_row_leftmost(*nary.get_first_child(), nary), nary.get_first_child());
    QCOMPARE(same_row_leftmost(*binaryL.get_first_child(), binaryL), binaryL.get_first_child());
    QCOMPARE(same_row_leftmost(*binaryR.get_first_child(), binaryR), binaryR.get_first_child());

    QCOMPARE(same_row_rightmost(nary, nary), &nary);
    QCOMPARE(same_row_rightmost(binaryL, binaryL), &binaryL);
    QCOMPARE(same_row_rightmost(binaryR, binaryR), &binaryR);
    QCOMPARE(same_row_rightmost(*nary.get_first_child(), nary), nary.get_first_child());
    QCOMPARE(same_row_rightmost(*binaryL.get_first_child(), binaryL), binaryL.get_first_child());
    QCOMPARE(same_row_rightmost(*binaryR.get_first_child(), binaryR), binaryR.get_first_child());

    QCOMPARE(deepest_rightmost_child(nary), nary.get_last_child());
    QCOMPARE(deepest_rightmost_child(binaryL), binaryL.get_left_child());
    QCOMPARE(deepest_rightmost_child(binaryR), binaryR.get_right_child());
}

void UtilityTest::multipleChildren() {
    auto twoChildren   = n('a')(n('b'), n('c'));
    auto multiChildren = n('d')(n('e'), n('f'), n('g'), n('h'), n('i'), n('j'));
    binary_node<char> binary(twoChildren);
    nary_node<char> nary(multiChildren);
    const nary_node<char>* children[] = {
        nary.get_child(0),
        nary.get_child(1),
        nary.get_child(2),
        nary.get_child(3),
        nary.get_child(4),
        nary.get_child(5)};

    QCOMPARE(left_branch_node(*binary.get_left_child(), binary), nullptr);
    QCOMPARE(left_branch_node(*binary.get_right_child(), binary), binary.get_left_child());
    QCOMPARE(left_branch_node(*children[5], nary), children[4]);
    QCOMPARE(left_branch_node(*children[4], nary), children[3]);
    QCOMPARE(left_branch_node(*children[3], nary), children[2]);
    QCOMPARE(left_branch_node(*children[2], nary), children[1]);
    QCOMPARE(left_branch_node(*children[1], nary), children[0]);
    QCOMPARE(left_branch_node(*children[0], nary), nullptr);

    QCOMPARE(right_branch_node(*binary.get_left_child(), binary), binary.get_right_child());
    QCOMPARE(right_branch_node(*binary.get_right_child(), binary), nullptr);
    QCOMPARE(right_branch_node(*children[5], nary), nullptr);
    QCOMPARE(right_branch_node(*children[4], nary), children[5]);
    QCOMPARE(right_branch_node(*children[3], nary), children[4]);
    QCOMPARE(right_branch_node(*children[2], nary), children[3]);
    QCOMPARE(right_branch_node(*children[1], nary), children[2]);
    QCOMPARE(right_branch_node(*children[0], nary), children[1]);

    QCOMPARE(same_row_leftmost(*children[5], nary), children[0]);
    QCOMPARE(same_row_leftmost(*children[4], nary), children[0]);
    QCOMPARE(same_row_leftmost(*children[3], nary), children[0]);
    QCOMPARE(same_row_leftmost(*children[2], nary), children[0]);
    QCOMPARE(same_row_leftmost(*children[1], nary), children[0]);
    QCOMPARE(same_row_leftmost(*children[0], nary), children[0]);
    QCOMPARE(same_row_leftmost(nary, nary), &nary);
    QCOMPARE(same_row_leftmost(*binary.get_left_child(), binary), binary.get_left_child());
    QCOMPARE(same_row_leftmost(*binary.get_right_child(), binary), binary.get_left_child());
    QCOMPARE(same_row_leftmost(binary, binary), &binary);

    QCOMPARE(same_row_rightmost(*children[5], nary), children[5]);
    QCOMPARE(same_row_rightmost(*children[4], nary), children[5]);
    QCOMPARE(same_row_rightmost(*children[3], nary), children[5]);
    QCOMPARE(same_row_rightmost(*children[2], nary), children[5]);
    QCOMPARE(same_row_rightmost(*children[1], nary), children[5]);
    QCOMPARE(same_row_rightmost(*children[0], nary), children[5]);
    QCOMPARE(same_row_rightmost(nary, nary), &nary);
    QCOMPARE(same_row_rightmost(*binary.get_left_child(), binary), binary.get_right_child());
    QCOMPARE(same_row_rightmost(*binary.get_right_child(), binary), binary.get_right_child());
    QCOMPARE(same_row_rightmost(binary, binary), &binary);

    QCOMPARE(deepest_rightmost_child(nary), children[5]);
    QCOMPARE(deepest_rightmost_child(binary), binary.get_right_child());
}

void UtilityTest::advancedBinary() {
    binary_node<int> node(
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
                                n(14))))))));
    const binary_node<int>* n2  = node.get_right_child();
    const binary_node<int>* n3  = n2->get_left_child();
    const binary_node<int>* n6  = n3->get_right_child();
    const binary_node<int>* n4  = n2->get_right_child();
    const binary_node<int>* n5  = n3->get_left_child();
    const binary_node<int>* n7  = n4->get_left_child();
    const binary_node<int>* n8  = n4->get_right_child();
    const binary_node<int>* n9  = n5->get_left_child();
    const binary_node<int>* n10 = n5->get_right_child();
    const binary_node<int>* n11 = n8->get_left_child();
    const binary_node<int>* n12 = n8->get_right_child();
    const binary_node<int>* n13 = n12->get_right_child();
    const binary_node<int>* n14 = n13->get_left_child();

    QCOMPARE(left_branch_node(*n8, node), n7);
    QCOMPARE(left_branch_node(*n8, *n2), n7);
    QCOMPARE(left_branch_node(*n8, *n4), n7);
    QCOMPARE(left_branch_node(*n8, *n8), nullptr);
    QCOMPARE(left_branch_node(*n7, node), n6);
    QCOMPARE(left_branch_node(*n7, *n2), n6);
    QCOMPARE(left_branch_node(*n7, *n4), nullptr);
    QCOMPARE(left_branch_node(*n7, *n7), nullptr);
    QCOMPARE(left_branch_node(*n12, node), n11);
    QCOMPARE(left_branch_node(*n12, *n2), n11);
    QCOMPARE(left_branch_node(*n12, *n4), n11);
    QCOMPARE(left_branch_node(*n12, *n8), n11);
    QCOMPARE(left_branch_node(*n12, *n12), nullptr);
    QCOMPARE(left_branch_node(*n11, node), n10);
    QCOMPARE(left_branch_node(*n11, *n2), n10);
    QCOMPARE(left_branch_node(*n11, *n4), nullptr);
    QCOMPARE(left_branch_node(*n13, node), nullptr);
    QCOMPARE(left_branch_node(*n13, *n8), nullptr);
    QCOMPARE(left_branch_node(*n14, node), nullptr);
    QCOMPARE(left_branch_node(*n9, node), nullptr);
    QCOMPARE(left_branch_node(*n5, node), nullptr);
    QCOMPARE(left_branch_node(*n3, node), nullptr);
    QCOMPARE(left_branch_node(node, node), nullptr);

    QCOMPARE(right_branch_node(node, node), nullptr);
    QCOMPARE(right_branch_node(*n2, node), nullptr);
    QCOMPARE(right_branch_node(*n2, *n2), nullptr);
    QCOMPARE(right_branch_node(*n3, node), n4);
    QCOMPARE(right_branch_node(*n3, *n2), n4);
    QCOMPARE(right_branch_node(*n3, *n3), nullptr);
    QCOMPARE(right_branch_node(*n5, node), n6);
    QCOMPARE(right_branch_node(*n5, *n2), n6);
    QCOMPARE(right_branch_node(*n5, *n3), n6);
    QCOMPARE(right_branch_node(*n5, *n5), nullptr);
    QCOMPARE(right_branch_node(*n6, node), n7);
    QCOMPARE(right_branch_node(*n6, *n2), n7);
    QCOMPARE(right_branch_node(*n6, *n3), nullptr);
    QCOMPARE(right_branch_node(*n10, node), n11);
    QCOMPARE(right_branch_node(*n10, *n2), n11);
    QCOMPARE(right_branch_node(*n10, *n3), nullptr);
    QCOMPARE(right_branch_node(*n11, node), n12);
    QCOMPARE(right_branch_node(*n11, *n2), n12);
    QCOMPARE(right_branch_node(*n11, *n4), n12);
    QCOMPARE(right_branch_node(*n11, *n8), n12);
    QCOMPARE(right_branch_node(*n11, *n11), nullptr);

    QCOMPARE(same_row_leftmost(node, node), &node);
    QCOMPARE(same_row_leftmost(*n2, node), n2);
    QCOMPARE(same_row_leftmost(*n2, *n2), n2);
    QCOMPARE(same_row_leftmost(*n3, node), n3);
    QCOMPARE(same_row_leftmost(*n3, *n2), n3);
    QCOMPARE(same_row_leftmost(*n3, *n3), n3);
    QCOMPARE(same_row_leftmost(*n4, node), n3);
    QCOMPARE(same_row_leftmost(*n4, *n2), n3);
    QCOMPARE(same_row_leftmost(*n4, *n4), n4);
    QCOMPARE(same_row_leftmost(*n7, node), n5);
    QCOMPARE(same_row_leftmost(*n7, *n2), n5);
    QCOMPARE(same_row_leftmost(*n7, *n4), n7);
    QCOMPARE(same_row_leftmost(*n7, *n7), n7);
    QCOMPARE(same_row_leftmost(*n8, node), n5);
    QCOMPARE(same_row_leftmost(*n8, *n2), n5);
    QCOMPARE(same_row_leftmost(*n8, *n4), n7);
    QCOMPARE(same_row_leftmost(*n8, *n8), n8);
    QCOMPARE(same_row_leftmost(*n12, node), n9);
    QCOMPARE(same_row_leftmost(*n12, *n2), n9);
    QCOMPARE(same_row_leftmost(*n12, *n4), n11);
    QCOMPARE(same_row_leftmost(*n12, *n8), n11);
    QCOMPARE(same_row_leftmost(*n12, *n12), n12);

    QCOMPARE(same_row_rightmost(node, node), &node);
    QCOMPARE(same_row_rightmost(*n9, node), n12);
    QCOMPARE(same_row_rightmost(*n9, *n2), n12);
    QCOMPARE(same_row_rightmost(*n9, *n3), n10);
    QCOMPARE(same_row_rightmost(*n9, *n5), n10);
    QCOMPARE(same_row_rightmost(*n9, *n9), n9);
    QCOMPARE(same_row_rightmost(*n10, node), n12);
    QCOMPARE(same_row_rightmost(*n10, *n2), n12);
    QCOMPARE(same_row_rightmost(*n10, *n3), n10);
    QCOMPARE(same_row_rightmost(*n10, *n5), n10);
    QCOMPARE(same_row_rightmost(*n10, *n10), n10);
    QCOMPARE(same_row_rightmost(*n11, node), n12);
    QCOMPARE(same_row_rightmost(*n11, *n2), n12);
    QCOMPARE(same_row_rightmost(*n11, *n4), n12);
    QCOMPARE(same_row_rightmost(*n11, *n8), n12);
    QCOMPARE(same_row_rightmost(*n11, *n11), n11);
    QCOMPARE(same_row_rightmost(*n14, node), n14);
    QCOMPARE(same_row_rightmost(*n14, *n2), n14);
    QCOMPARE(same_row_rightmost(*n14, *n4), n14);
    QCOMPARE(same_row_rightmost(*n14, *n8), n14);
    QCOMPARE(same_row_rightmost(*n14, *n12), n14);
    QCOMPARE(same_row_rightmost(*n14, *n13), n14);
    QCOMPARE(same_row_rightmost(*n14, *n14), n14);
    QCOMPARE(same_row_rightmost(*n13, node), n13);
    QCOMPARE(same_row_rightmost(*n13, *n2), n13);
    QCOMPARE(same_row_rightmost(*n13, *n4), n13);
    QCOMPARE(same_row_rightmost(*n13, *n8), n13);
    QCOMPARE(same_row_rightmost(*n13, *n12), n13);
    QCOMPARE(same_row_rightmost(*n13, *n13), n13);

    QCOMPARE(deepest_rightmost_child(node), n14);
    QCOMPARE(deepest_rightmost_child(*n3), n10);
}

void UtilityTest::advancedNary() {
    const nary_node<int> node(
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
                    n(16)))));
    const nary_node<int>* n2  = (&node)->get_child(0);
    const nary_node<int>* n3  = (&node)->get_child(1);
    const nary_node<int>* n4  = (&node)->get_child(2);
    const nary_node<int>* n6  = n2->get_child(1);
    const nary_node<int>* n9  = n3->get_child(1);
    const nary_node<int>* n10 = n4->get_child(0);
    const nary_node<int>* n11 = n4->get_child(1);
    const nary_node<int>* n12 = n6->get_child(0);
    const nary_node<int>* n13 = n6->get_child(1);
    const nary_node<int>* n14
        = n3
              ->get_child(0)  // 8
              ->get_child(0); // 14
    const nary_node<int>* n15 = n11->get_child(0);
    const nary_node<int>* n16 = n11->get_child(1);
    const nary_node<int>* n18 = n12->get_child(1);
    const nary_node<int>* n19 = n13->get_child(0);
    const nary_node<int>* n20 = n15->get_child(0);
    const nary_node<int>* n23 = n18->get_child(0);
    const nary_node<int>* n24
        = n15
              ->get_child(1)  // 21
              ->get_child(0); // 24
    const nary_node<int>* n25
        = n15
              ->get_child(1)  // n21
              ->get_child(1); // n25
    const nary_node<int>* n26 = n24->get_child(0);
    const nary_node<int>* n27 = n26->get_child(0);

    QCOMPARE(left_branch_node(node, node), nullptr);
    QCOMPARE(left_branch_node(*n10, node), n9);
    QCOMPARE(left_branch_node(*n10, *n4), nullptr);
    QCOMPARE(left_branch_node(*n10, *n10), nullptr);
    QCOMPARE(left_branch_node(*n12, node), nullptr);
    QCOMPARE(left_branch_node(*n12, *n2), nullptr);
    QCOMPARE(left_branch_node(*n12, *n12), nullptr);
    QCOMPARE(left_branch_node(*n15, node), n14);
    QCOMPARE(left_branch_node(*n15, *n4), nullptr);
    QCOMPARE(left_branch_node(*n15, *n11), nullptr);
    QCOMPARE(left_branch_node(*n20, node), n19);
    QCOMPARE(left_branch_node(*n20, *n4), nullptr);
    QCOMPARE(left_branch_node(*n23, node), nullptr);
    QCOMPARE(left_branch_node(*n24, node), n23);
    QCOMPARE(left_branch_node(*n26, node), nullptr);
    QCOMPARE(left_branch_node(*n27, node), nullptr);

    QCOMPARE(same_row_rightmost(node, node), &node);
    QCOMPARE(same_row_rightmost(*n2, node), n4);
    QCOMPARE(same_row_rightmost(*n2, *n2), n2);
    QCOMPARE(same_row_rightmost(*n4, node), n4);
    QCOMPARE(same_row_rightmost(*n4, *n4), n4);
    QCOMPARE(same_row_rightmost(*n9, node), n11);
    QCOMPARE(same_row_rightmost(*n9, *n3), n9);
    QCOMPARE(same_row_rightmost(*n9, *n9), n9);
    QCOMPARE(same_row_rightmost(*n12, node), n16);
    QCOMPARE(same_row_rightmost(*n12, *n2), n13);
    QCOMPARE(same_row_rightmost(*n12, *n6), n13);
    QCOMPARE(same_row_rightmost(*n12, *n12), n12);
    QCOMPARE(same_row_rightmost(*n23, node), n25);
    QCOMPARE(same_row_rightmost(*n23, *n2), n23);
    QCOMPARE(same_row_rightmost(*n23, *n6), n23);
    QCOMPARE(same_row_rightmost(*n23, *n6), n23);
    QCOMPARE(same_row_rightmost(*n23, *n12), n23);
    QCOMPARE(same_row_rightmost(*n23, *n18), n23);
    QCOMPARE(same_row_rightmost(*n23, *n23), n23);
    QCOMPARE(same_row_rightmost(*n27, node), n27);
    QCOMPARE(same_row_rightmost(*n27, *n4), n27);
    QCOMPARE(same_row_rightmost(*n27, *n11), n27);
    QCOMPARE(same_row_rightmost(*n27, *n15), n27);
    QCOMPARE(same_row_rightmost(*n27, *n24), n27);
    QCOMPARE(same_row_rightmost(*n27, *n26), n27);
    QCOMPARE(same_row_rightmost(*n27, *n27), n27);

    QCOMPARE(deepest_rightmost_child(node), n27);
}

void UtilityTest::isTagOfPolicyTest() {
    QVERIFY((is_tag_of_policy<
             policy::pre_order,
             binary_node<int>,
             std::allocator<int>>));

    QVERIFY((!is_tag_of_policy<
             pre_order_impl<binary_node<int>>,
             binary_node<int>,
             std::allocator<int>>));

    QVERIFY((is_tag_of_policy<
             policy::in_order,
             binary_node<int>,
             std::allocator<int>>));

    QVERIFY((!is_tag_of_policy<
             in_order_impl<nary_node<int>>,
             binary_node<int>,
             std::allocator<int>>));

    QVERIFY((is_tag_of_policy<
             policy::post_order,
             nary_node<int>,
             std::allocator<int>>));

    QVERIFY((!is_tag_of_policy<
             post_order_impl<nary_node<int>>,
             nary_node<int>,
             std::allocator<int>>));

    QVERIFY((is_tag_of_policy<
             policy::breadth_first,
             nary_node<int>,
             std::allocator<int>>));

    QVERIFY((!is_tag_of_policy<
             breadth_first_impl<nary_node<int>>,
             nary_node<int>,
             std::allocator<int>>));
}

QTEST_MAIN(UtilityTest);
#include "UtilityTest.moc"
