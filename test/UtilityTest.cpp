#include <QtTest/QtTest>

#include <TreeDS/node/binary_node.hpp>
#include <TreeDS/node/nary_node.hpp>
#include <TreeDS/utility.hpp>

using namespace std;
using namespace ds;

class UtilityTest : public QObject {

    Q_OBJECT

    private slots:
    void singleNode();
    void oneChild();
    void multipleChildren();
    void advancedBinary();
    void advancedNary();
};

void UtilityTest::singleNode() {
    auto single = n(123);
    nary_node<int> nary(single);
    binary_node<int> binary(single);

    QCOMPARE(prev_branch_sibling(nary), nullptr);
    QCOMPARE(prev_branch_sibling(binary), nullptr);

    QCOMPARE(upper_row_rightmost(nary), nullptr);
    QCOMPARE(upper_row_rightmost(binary), nullptr);

    QCOMPARE(deepest_rightmost_child(nary), &nary);
    QCOMPARE(deepest_rightmost_child(binary), &binary);
}

void UtilityTest::oneChild() {
    auto leftChild  = n(234)(n(345));
    auto rightChild = n(456)(n(), n(567));
    nary_node<int> nary(leftChild);
    binary_node<int> binaryL(leftChild);
    binary_node<int> binaryR(rightChild);

    QCOMPARE(prev_branch_sibling(nary), nullptr);
    QCOMPARE(prev_branch_sibling(binaryL), nullptr);
    QCOMPARE(prev_branch_sibling(binaryR), nullptr);
    QCOMPARE(prev_branch_sibling(*nary.get_first_child()), nullptr);
    QCOMPARE(prev_branch_sibling(*binaryL.get_first_child()), nullptr);
    QCOMPARE(prev_branch_sibling(*binaryR.get_first_child()), nullptr);

    QCOMPARE(upper_row_rightmost(nary), nullptr);
    QCOMPARE(upper_row_rightmost(binaryL), nullptr);
    QCOMPARE(upper_row_rightmost(binaryR), nullptr);
    QCOMPARE(upper_row_rightmost(*nary.get_first_child()), &nary);
    QCOMPARE(upper_row_rightmost(*binaryL.get_first_child()), &binaryL);
    QCOMPARE(upper_row_rightmost(*binaryR.get_first_child()), &binaryR);

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

    QCOMPARE(prev_branch_sibling(*binary.get_right_child()), binary.get_left_child());
    QCOMPARE(prev_branch_sibling(*children[5]), children[4]);
    QCOMPARE(prev_branch_sibling(*children[4]), children[3]);
    QCOMPARE(prev_branch_sibling(*children[3]), children[2]);
    QCOMPARE(prev_branch_sibling(*children[2]), children[1]);
    QCOMPARE(prev_branch_sibling(*children[1]), children[0]);
    QCOMPARE(prev_branch_sibling(*children[0]), nullptr);

    QCOMPARE(upper_row_rightmost(*children[5]), &nary);
    QCOMPARE(upper_row_rightmost(*children[4]), &nary);
    QCOMPARE(upper_row_rightmost(*children[3]), &nary);
    QCOMPARE(upper_row_rightmost(*children[2]), &nary);
    QCOMPARE(upper_row_rightmost(*children[1]), &nary);
    QCOMPARE(upper_row_rightmost(*children[0]), &nary);
    QCOMPARE(upper_row_rightmost(nary), nullptr);
    QCOMPARE(upper_row_rightmost(*binary.get_left_child()), &binary);
    QCOMPARE(upper_row_rightmost(*binary.get_right_child()), &binary);
    QCOMPARE(upper_row_rightmost(binary), nullptr);

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

    QCOMPARE(prev_branch_sibling(*n8), n7);
    QCOMPARE(prev_branch_sibling(*n7), n6);
    QCOMPARE(prev_branch_sibling(*n12), n11);
    QCOMPARE(prev_branch_sibling(*n11), n10);
    QCOMPARE(prev_branch_sibling(*n13), nullptr);
    QCOMPARE(prev_branch_sibling(*n14), nullptr);
    QCOMPARE(prev_branch_sibling(*n9), nullptr);
    QCOMPARE(prev_branch_sibling(*n5), nullptr);
    QCOMPARE(prev_branch_sibling(*n3), nullptr);
    QCOMPARE(prev_branch_sibling(node), nullptr);

    QCOMPARE(upper_row_rightmost(*n9), n8);
    QCOMPARE(upper_row_rightmost(*n10), n8);
    QCOMPARE(upper_row_rightmost(*n11), n8);
    QCOMPARE(upper_row_rightmost(*n14), n13);
    QCOMPARE(upper_row_rightmost(*n13), n12);
    QCOMPARE(upper_row_rightmost(*n13), n12);

    QCOMPARE(deepest_rightmost_child(node), n14);
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
    const nary_node<int>* n4 = (&node)->get_child(2);
    const nary_node<int>* n9
        = (&node)
              ->get_child(1)  // 3
              ->get_child(1); // 9
    const nary_node<int>* n10 = n4->get_child(0);
    const nary_node<int>* n11 = n4->get_child(1);
    const nary_node<int>* n12
        = n2
              ->get_child(1)  // 6
              ->get_child(0); //12
    const nary_node<int>* n14
        = (&node)
              ->get_child(1)  // 3
              ->get_child(0)  // 8
              ->get_child(0); // 14
    const nary_node<int>* n15
        = n4
              ->get_child(1)  // 11
              ->get_child(0); // 15
    const nary_node<int>* n19
        = n2
              ->get_child(1)  // 6
              ->get_child(1)  // 13
              ->get_child(0); // 19
    const nary_node<int>* n20 = n15->get_child(0);
    const nary_node<int>* n22 = n15->get_child(2);
    const nary_node<int>* n23
        = n2
              ->get_child(1)  // 6
              ->get_child(0)  // 12
              ->get_child(1)  // 18
              ->get_child(0); // 23
    const nary_node<int>* n24
        = n15
              ->get_child(1)  // 22
              ->get_child(0); // 24
    const nary_node<int>* n26 = n24->get_child(0);
    const nary_node<int>* n27 = n26->get_child(0);

    QCOMPARE(prev_branch_sibling(*n10), n9);
    QCOMPARE(prev_branch_sibling(*n12), nullptr);
    QCOMPARE(prev_branch_sibling(*n15), n14);
    QCOMPARE(prev_branch_sibling(*n20), n19);
    QCOMPARE(prev_branch_sibling(*n23), nullptr);
    QCOMPARE(prev_branch_sibling(*n24), n23);
    QCOMPARE(prev_branch_sibling(*n26), nullptr);
    QCOMPARE(prev_branch_sibling(*n27), nullptr);

    QCOMPARE(upper_row_rightmost(node), nullptr);
    QCOMPARE(upper_row_rightmost(*n2), &node);
    QCOMPARE(upper_row_rightmost(*n4), &node);
    QCOMPARE(upper_row_rightmost(*n9), n4);
    QCOMPARE(upper_row_rightmost(*n12), n11);
    QCOMPARE(upper_row_rightmost(*n23), n22);
    QCOMPARE(upper_row_rightmost(*n27), n26);

    QCOMPARE(deepest_rightmost_child(node), n27);
}

QTEST_MAIN(UtilityTest);
#include "UtilityTest.moc"
