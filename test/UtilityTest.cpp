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
    void updateableTest();
    void isTagOfPolicyTest();
};

void UtilityTest::singleNode() {
    auto single = n(123);
    nary_node<int> nary(single);
    binary_node<int> binary(single);

    QCOMPARE(prev_branch_sibling(nary, nary), nullptr);
    QCOMPARE(prev_branch_sibling(binary, binary), nullptr);

    QCOMPARE(upper_row_rightmost(nary, nary), nullptr);
    QCOMPARE(upper_row_rightmost(binary, binary), nullptr);

    QCOMPARE(deepest_rightmost_child(nary), &nary);
    QCOMPARE(deepest_rightmost_child(binary), &binary);
}

void UtilityTest::oneChild() {
    auto leftChild  = n(234)(n(345));
    auto rightChild = n(456)(n(), n(567));
    nary_node<int> nary(leftChild);
    binary_node<int> binaryL(leftChild);
    binary_node<int> binaryR(rightChild);

    QCOMPARE(prev_branch_sibling(nary, nary), nullptr);
    QCOMPARE(prev_branch_sibling(binaryL, binaryL), nullptr);
    QCOMPARE(prev_branch_sibling(binaryR, binaryR), nullptr);
    QCOMPARE(prev_branch_sibling(*nary.get_first_child(), nary), nullptr);
    QCOMPARE(prev_branch_sibling(*binaryL.get_first_child(), binaryL), nullptr);
    QCOMPARE(prev_branch_sibling(*binaryR.get_first_child(), binaryR), nullptr);

    QCOMPARE(upper_row_rightmost(nary, nary), nullptr);
    QCOMPARE(upper_row_rightmost(binaryL, binaryL), nullptr);
    QCOMPARE(upper_row_rightmost(binaryR, binaryR), nullptr);
    QCOMPARE(upper_row_rightmost(*nary.get_first_child(), nary), &nary);
    QCOMPARE(upper_row_rightmost(*binaryL.get_first_child(), binaryL), &binaryL);
    QCOMPARE(upper_row_rightmost(*binaryR.get_first_child(), binaryR), &binaryR);

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

    QCOMPARE(prev_branch_sibling(*binary.get_right_child(), binary), binary.get_left_child());
    QCOMPARE(prev_branch_sibling(*children[5], nary), children[4]);
    QCOMPARE(prev_branch_sibling(*children[4], nary), children[3]);
    QCOMPARE(prev_branch_sibling(*children[3], nary), children[2]);
    QCOMPARE(prev_branch_sibling(*children[2], nary), children[1]);
    QCOMPARE(prev_branch_sibling(*children[1], nary), children[0]);
    QCOMPARE(prev_branch_sibling(*children[0], nary), nullptr);

    QCOMPARE(upper_row_rightmost(*children[5], nary), &nary);
    QCOMPARE(upper_row_rightmost(*children[4], nary), &nary);
    QCOMPARE(upper_row_rightmost(*children[3], nary), &nary);
    QCOMPARE(upper_row_rightmost(*children[2], nary), &nary);
    QCOMPARE(upper_row_rightmost(*children[1], nary), &nary);
    QCOMPARE(upper_row_rightmost(*children[0], nary), &nary);
    QCOMPARE(upper_row_rightmost(nary, nary), nullptr);
    QCOMPARE(upper_row_rightmost(*binary.get_left_child(), binary), &binary);
    QCOMPARE(upper_row_rightmost(*binary.get_right_child(), binary), &binary);
    QCOMPARE(upper_row_rightmost(binary, binary), nullptr);

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
    const binary_node<int>* n2 = node.get_right_child();
    const binary_node<int>* n3
        = (&node)
              ->get_right_child() // 2
              ->get_left_child(); // 3
    const binary_node<int>* n6 = n3->get_right_child();
    const binary_node<int>* n4
        = (&node)
              ->get_right_child()  // 2
              ->get_right_child(); // 4
    const binary_node<int>* n5  = n3->get_left_child();
    const binary_node<int>* n7  = n4->get_left_child();
    const binary_node<int>* n8  = n4->get_right_child();
    const binary_node<int>* n9  = n5->get_left_child();
    const binary_node<int>* n10 = n5->get_right_child();
    const binary_node<int>* n11 = n8->get_left_child();
    const binary_node<int>* n12
        = n4
              ->get_right_child()  // 8
              ->get_right_child(); // 11
    const binary_node<int>* n13 = n12->get_right_child();
    const binary_node<int>* n14 = n13->get_left_child();

    QCOMPARE(prev_branch_sibling(*n8, node), n7);
    QCOMPARE(prev_branch_sibling(*n8, *n2), n7);
    QCOMPARE(prev_branch_sibling(*n8, *n4), n7);
    QCOMPARE(prev_branch_sibling(*n8, *n8), nullptr);
    QCOMPARE(prev_branch_sibling(*n7, node), n6);
    QCOMPARE(prev_branch_sibling(*n7, *n2), n6);
    QCOMPARE(prev_branch_sibling(*n7, *n4), nullptr);
    QCOMPARE(prev_branch_sibling(*n7, *n7), nullptr);
    QCOMPARE(prev_branch_sibling(*n12, node), n11);
    QCOMPARE(prev_branch_sibling(*n12, *n2), n11);
    QCOMPARE(prev_branch_sibling(*n12, *n4), n11);
    QCOMPARE(prev_branch_sibling(*n12, *n8), n11);
    QCOMPARE(prev_branch_sibling(*n12, *n12), nullptr);
    QCOMPARE(prev_branch_sibling(*n11, node), n10);
    QCOMPARE(prev_branch_sibling(*n11, *n2), n10);
    QCOMPARE(prev_branch_sibling(*n11, *n4), nullptr);
    QCOMPARE(prev_branch_sibling(*n13, node), nullptr);
    QCOMPARE(prev_branch_sibling(*n13, *n8), nullptr);
    QCOMPARE(prev_branch_sibling(*n14, node), nullptr);
    QCOMPARE(prev_branch_sibling(*n9, node), nullptr);
    QCOMPARE(prev_branch_sibling(*n5, node), nullptr);
    QCOMPARE(prev_branch_sibling(*n3, node), nullptr);
    QCOMPARE(prev_branch_sibling(node, node), nullptr);

    QCOMPARE(upper_row_rightmost(node, node), nullptr);
    QCOMPARE(upper_row_rightmost(*n9, node), n8);
    QCOMPARE(upper_row_rightmost(*n9, *n2), n8);
    QCOMPARE(upper_row_rightmost(*n9, *n2), n8);
    QCOMPARE(upper_row_rightmost(*n9, *n3), n6);
    QCOMPARE(upper_row_rightmost(*n9, *n5), n5);
    QCOMPARE(upper_row_rightmost(*n9, *n9), nullptr);
    QCOMPARE(upper_row_rightmost(*n10, node), n8);
    QCOMPARE(upper_row_rightmost(*n10, *n2), n8);
    QCOMPARE(upper_row_rightmost(*n10, *n3), n6);
    QCOMPARE(upper_row_rightmost(*n10, *n5), n5);
    QCOMPARE(upper_row_rightmost(*n10, *n10), nullptr);
    QCOMPARE(upper_row_rightmost(*n11, node), n8);
    QCOMPARE(upper_row_rightmost(*n11, *n2), n8);
    QCOMPARE(upper_row_rightmost(*n11, *n4), n8);
    QCOMPARE(upper_row_rightmost(*n11, *n8), n8);
    QCOMPARE(upper_row_rightmost(*n11, *n11), nullptr);
    QCOMPARE(upper_row_rightmost(*n14, node), n13);
    QCOMPARE(upper_row_rightmost(*n14, *n2), n13);
    QCOMPARE(upper_row_rightmost(*n14, *n4), n13);
    QCOMPARE(upper_row_rightmost(*n14, *n8), n13);
    QCOMPARE(upper_row_rightmost(*n14, *n12), n13);
    QCOMPARE(upper_row_rightmost(*n14, *n13), n13);
    QCOMPARE(upper_row_rightmost(*n14, *n14), nullptr);
    QCOMPARE(upper_row_rightmost(*n13, node), n12);
    QCOMPARE(upper_row_rightmost(*n13, *n2), n12);
    QCOMPARE(upper_row_rightmost(*n13, *n4), n12);
    QCOMPARE(upper_row_rightmost(*n13, *n8), n12);
    QCOMPARE(upper_row_rightmost(*n13, *n12), n12);
    QCOMPARE(upper_row_rightmost(*n13, *n13), nullptr);

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
    const nary_node<int>* n2 = (&node)->get_child(0);
    const nary_node<int>* n3 = (&node)->get_child(1);
    const nary_node<int>* n4 = (&node)->get_child(2);
    const nary_node<int>* n6 = n2->get_child(1);
    const nary_node<int>* n7 = n2->get_child(2);
    const nary_node<int>* n9
        = (&node)
              ->get_child(1)  // 3
              ->get_child(1); // 9
    const nary_node<int>* n10 = n4->get_child(0);
    const nary_node<int>* n11 = n4->get_child(1);
    const nary_node<int>* n12 = n6->get_child(0);
    const nary_node<int>* n14
        = n3
              ->get_child(0)  // 8
              ->get_child(0); // 14
    const nary_node<int>* n15 = n11->get_child(0);
    const nary_node<int>* n18 = n12->get_child(1);
    const nary_node<int>* n19
        = n6
              ->get_child(1)  // 13
              ->get_child(0); // 19
    const nary_node<int>* n20 = n15->get_child(0);
    const nary_node<int>* n22 = n15->get_child(2);
    const nary_node<int>* n23 = n18->get_child(0);
    const nary_node<int>* n24
        = n15
              ->get_child(1)
              ->get_child(0);
    const nary_node<int>* n26 = n24->get_child(0);
    const nary_node<int>* n27 = n26->get_child(0);

    QCOMPARE(prev_branch_sibling(node, node), nullptr);
    QCOMPARE(prev_branch_sibling(*n10, node), n9);
    QCOMPARE(prev_branch_sibling(*n10, *n4), nullptr);
    QCOMPARE(prev_branch_sibling(*n10, *n10), nullptr);
    QCOMPARE(prev_branch_sibling(*n12, node), nullptr);
    QCOMPARE(prev_branch_sibling(*n12, *n2), nullptr);
    QCOMPARE(prev_branch_sibling(*n12, *n12), nullptr);
    QCOMPARE(prev_branch_sibling(*n15, node), n14);
    QCOMPARE(prev_branch_sibling(*n15, *n4), nullptr);
    QCOMPARE(prev_branch_sibling(*n15, *n11), nullptr);
    QCOMPARE(prev_branch_sibling(*n20, node), n19);
    QCOMPARE(prev_branch_sibling(*n20, *n4), nullptr);
    QCOMPARE(prev_branch_sibling(*n23, node), nullptr);
    QCOMPARE(prev_branch_sibling(*n24, node), n23);
    QCOMPARE(prev_branch_sibling(*n26, node), nullptr);
    QCOMPARE(prev_branch_sibling(*n27, node), nullptr);

    QCOMPARE(upper_row_rightmost(node, node), nullptr);
    QCOMPARE(upper_row_rightmost(*n2, node), &node);
    QCOMPARE(upper_row_rightmost(*n2, *n2), nullptr);
    QCOMPARE(upper_row_rightmost(*n4, node), &node);
    QCOMPARE(upper_row_rightmost(*n4, *n4), nullptr);
    QCOMPARE(upper_row_rightmost(*n9, node), n4);
    QCOMPARE(upper_row_rightmost(*n9, *n3), n3);
    QCOMPARE(upper_row_rightmost(*n9, *n9), nullptr);
    QCOMPARE(upper_row_rightmost(*n12, node), n11);
    QCOMPARE(upper_row_rightmost(*n12, *n2), n7);
    QCOMPARE(upper_row_rightmost(*n12, *n6), n6);
    QCOMPARE(upper_row_rightmost(*n12, *n12), nullptr);
    QCOMPARE(upper_row_rightmost(*n23, node), n22);
    QCOMPARE(upper_row_rightmost(*n23, *n2), n19);
    QCOMPARE(upper_row_rightmost(*n23, *n6), n19);
    QCOMPARE(upper_row_rightmost(*n23, *n6), n19);
    QCOMPARE(upper_row_rightmost(*n23, *n12), n18);
    QCOMPARE(upper_row_rightmost(*n23, *n18), n18);
    QCOMPARE(upper_row_rightmost(*n23, *n23), nullptr);
    QCOMPARE(upper_row_rightmost(*n27, node), n26);
    QCOMPARE(upper_row_rightmost(*n27, *n4), n26);
    QCOMPARE(upper_row_rightmost(*n27, *n11), n26);
    QCOMPARE(upper_row_rightmost(*n27, *n15), n26);
    QCOMPARE(upper_row_rightmost(*n27, *n24), n26);
    QCOMPARE(upper_row_rightmost(*n27, *n26), n26);
    QCOMPARE(upper_row_rightmost(*n27, *n27), nullptr);

    QCOMPARE(deepest_rightmost_child(node), n27);
}

void UtilityTest::updateableTest() {
    QVERIFY((
        is_updateable<
            breadth_first_impl<binary_node<int>>,
            binary_node<int>>::value));

    QVERIFY((
        !is_updateable<
            breadth_first_impl<binary_node<int>>,
            binary_node<float>>::value));

    QVERIFY((
        is_updateable<
            breadth_first_impl<binary_node<Foo>>,
            binary_node<Foo>>::value));

    QVERIFY((
        !is_updateable<
            breadth_first_impl<binary_node<Foo>>,
            binary_node<Bar>>::value));

    QVERIFY((
        is_updateable<
            breadth_first_impl<nary_node<string>>,
            nary_node<string>>::value));

    QVERIFY((
        !is_updateable<
            breadth_first_impl<nary_node<string>>,
            nary_node<char>>::value));

    QVERIFY((
        is_updateable<
            breadth_first_impl<nary_node<Bar>>,
            nary_node<Bar>>::value));
}

void UtilityTest::isTagOfPolicyTest() {
    QVERIFY((is_tag_of_policy<
             pre_order,
             binary_node<int>,
             std::allocator<int>>::value));

    QVERIFY((!is_tag_of_policy<
             pre_order_impl<binary_node<int>>,
             binary_node<int>,
             std::allocator<int>>::value));

    QVERIFY((is_tag_of_policy<
             in_order,
             nary_node<int>,
             std::allocator<int>>::value));

    QVERIFY((!is_tag_of_policy<
             in_order_impl<nary_node<int>>,
             nary_node<int>,
             std::allocator<int>>::value));

    QVERIFY((is_tag_of_policy<
             post_order,
             nary_node<int>,
             std::allocator<int>>::value));

    QVERIFY((!is_tag_of_policy<
             post_order_impl<nary_node<int>>,
             nary_node<int>,
             std::allocator<int>>::value));

    QVERIFY((is_tag_of_policy<
             breadth_first,
             nary_node<int>,
             std::allocator<int>>::value));

    QVERIFY((!is_tag_of_policy<
             breadth_first_impl<nary_node<int>>,
             nary_node<int>,
             std::allocator<int>>::value));
}

QTEST_MAIN(UtilityTest);
#include "UtilityTest.moc"
