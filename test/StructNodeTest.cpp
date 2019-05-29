#include <QtTest/QtTest>

#include <TreeDS/tree>

#include "Types.hpp"

using namespace std;
using namespace md;

class StructNodeTest : public QObject {

    Q_OBJECT

    private slots:
    void smallTrees();
    void bigTree1();
};

void StructNodeTest::smallTrees() {
    auto emptyNode = n();
    QCOMPARE(emptyNode.get_subtree_size(), 0);
    QCOMPARE(emptyNode.get_subtree_arity(), 0);
    QCOMPARE(emptyNode.get_index(), 0);

    auto singleNode = n(24);
    QCOMPARE(singleNode.get_value(), 24);
    QCOMPARE(singleNode.get_subtree_size(), 1);
    QCOMPARE(singleNode.get_subtree_arity(), 0);
    QCOMPARE(singleNode.get_index(), 0);

    auto leftChild = n(11)(n(57));
    QCOMPARE(leftChild.get_subtree_size(), 2);
    QCOMPARE(leftChild.get_subtree_arity(), 1);
    QCOMPARE(leftChild.get_value(), 11);
    QCOMPARE(get_child<0>(leftChild).get_value(), 57);
    QCOMPARE(leftChild.get_index(), 0);
    QCOMPARE(get_child<0>(leftChild).get_index(), 0);

    auto rightChild = n(97)(n(), n(33));
    QCOMPARE(rightChild.get_subtree_size(), 2);
    QCOMPARE(rightChild.get_subtree_arity(), 1);
    QCOMPARE(rightChild.get_value(), 97);
    QCOMPARE(get_child<1>(rightChild).get_value(), 33);
    QCOMPARE(rightChild.get_index(), 0);
    QCOMPARE(get_child<1>(rightChild).get_index(), 1);

    auto twoChildren = n(65)(n(82), n(21));
    QCOMPARE(twoChildren.get_subtree_size(), 3);
    QCOMPARE(twoChildren.get_subtree_arity(), 2);
    QCOMPARE(twoChildren.get_value(), 65);
    QCOMPARE(get_child<0>(twoChildren).get_value(), 82);
    QCOMPARE(get_child<1>(twoChildren).get_value(), 21);
    QCOMPARE(get_child<0>(twoChildren).get_index(), 0);
    QCOMPARE(get_child<1>(twoChildren).get_index(), 1);

    auto threeChildren = n(45)(n(39), n(71), n(18));
    QCOMPARE(threeChildren.get_subtree_size(), 4);
    QCOMPARE(threeChildren.get_subtree_arity(), 3);
    QCOMPARE(threeChildren.get_value(), 45);
    QCOMPARE(get_child<0>(threeChildren).get_value(), 39);
    QCOMPARE(get_child<1>(threeChildren).get_value(), 71);
    QCOMPARE(get_child<2>(threeChildren).get_value(), 18);
    QCOMPARE(get_child<0>(threeChildren).get_index(), 0);
    QCOMPARE(get_child<1>(threeChildren).get_index(), 1);
    QCOMPARE(get_child<2>(threeChildren).get_index(), 2);

    auto fourChildren = n(26)(n(20), n(41), n(15), n(76));
    QCOMPARE(fourChildren.get_subtree_size(), 5);
    QCOMPARE(fourChildren.get_subtree_arity(), 4);
    QCOMPARE(fourChildren.get_value(), 26);
    QCOMPARE(get_child<0>(fourChildren).get_value(), 20);
    QCOMPARE(get_child<1>(fourChildren).get_value(), 41);
    QCOMPARE(get_child<2>(fourChildren).get_value(), 15);
    QCOMPARE(get_child<3>(fourChildren).get_value(), 76);
    QCOMPARE(get_child<0>(fourChildren).get_index(), 0);
    QCOMPARE(get_child<1>(fourChildren).get_index(), 1);
    QCOMPARE(get_child<2>(fourChildren).get_index(), 2);
    QCOMPARE(get_child<3>(fourChildren).get_index(), 3);

    // Remember that calling the operator() discard the previous children, that will be replaced by the arguments.
    auto composedTree = threeChildren(
        rightChild(
            twoChildren),
        fourChildren,
        singleNode(
            leftChild,
            rightChild,
            emptyNode, // Remember that empty nodes not participate to size_value nor arity.
            fourChildren,
            threeChildren,
            fourChildren));
    QCOMPARE(composedTree.get_subtree_size(), 29);
    QCOMPARE(composedTree.get_subtree_arity(), 5);
    QCOMPARE(composedTree.get_value(), 45);
    QCOMPARE(get_child<0>(composedTree).get_value(), 97);
    QCOMPARE(
        get_child<0>(
            get_child<0>(composedTree))
            .get_value(),
        65);
    QCOMPARE(get_child<0>(composedTree).get_value(), 97);
    QCOMPARE(
        get_child<1>(
            get_child<3>(
                get_child<2>(composedTree)))
            .get_value(),
        41);
    QCOMPARE(get_child<1>(composedTree).get_value(), 26);
    QCOMPARE(get_child<2>(composedTree).get_value(), 24);
}

void StructNodeTest::bigTree1() {
    auto tree
        = n(50)(
            n(8001)(
                n(1),
                n(2)(
                    n(109)(
                        n(777)(
                            n(901)(
                                n(334)(
                                    n(112)(
                                        n(623)(
                                            n(895)(
                                                n(10023))))))))),
                n(3),
                n(4),
                n(),
                n(6),
                n(7),
                n(8)(
                    n(6574),
                    n(),
                    n(98235),
                    n(564)),
                n(9),
                n(10),
                n(),
                n(12),
                n(13),
                n(14),
                n()));
    QCOMPARE(tree.get_subtree_size(), 25);
    QCOMPARE(tree.get_subtree_arity(), 12);
    QCOMPARE(tree.get_value(), 50);
    QCOMPARE(get_child<0>(tree).get_value(), 8001);
    QCOMPARE(get_child<1>(get_child<0>(tree)).get_subtree_size(), 9);
    QCOMPARE(get_child<1>(get_child<0>(tree)).get_subtree_arity(), 1);
    QCOMPARE(
        get_child<2>(
            get_child<7>(
                get_child<0>(tree)))
            .get_value(),
        98235);
    QCOMPARE(get_child<6>(get_child<0>(tree)).get_index(), 6);
    QCOMPARE(get_child<4>(get_child<0>(tree)).get_index(), 4);
    QCOMPARE(get_child<14>(get_child<0>(tree)).get_index(), 14);
    QCOMPARE(get_child<7>(get_child<0>(tree)).get_subtree_size(), 4);
    QCOMPARE(get_child<7>(get_child<0>(tree)).get_subtree_arity(), 3);
    QCOMPARE(get_child<7>(get_child<0>(tree)).get_index(), 7);
}

QTEST_MAIN(StructNodeTest);
#include "StructNodeTest.moc"
