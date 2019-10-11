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
    void emtyNodes();
};

void StructNodeTest::smallTrees() {
    auto emptyNode = n();
    QCOMPARE(emptyNode.subtree_size(), 0);
    QCOMPARE(emptyNode.subtree_arity(), 0);
    QCOMPARE(emptyNode.get_index(), 0);

    auto singleNode = n(24);
    QCOMPARE(singleNode.get_value(), 24);
    QCOMPARE(singleNode.subtree_size(), 1);
    QCOMPARE(singleNode.subtree_arity(), 0);
    QCOMPARE(singleNode.get_index(), 0);

    auto leftChild = n(11)(n(57));
    QCOMPARE(leftChild.subtree_size(), 2);
    QCOMPARE(leftChild.subtree_arity(), 1);
    QCOMPARE(leftChild.get_value(), 11);
    QCOMPARE(leftChild.get_child(const_index<0>()).get_value(), 57);
    QCOMPARE(leftChild.get_index(), 0);
    QCOMPARE(leftChild.get_child(const_index<0>()).get_index(), 0);

    auto rightChild = n(97)(n(), n(33));
    QCOMPARE(rightChild.subtree_size(), 2);
    QCOMPARE(rightChild.subtree_arity(), 1);
    QCOMPARE(rightChild.get_value(), 97);
    QCOMPARE(rightChild.get_child(const_index<1>()).get_value(), 33);
    QCOMPARE(rightChild.get_index(), 0);
    QCOMPARE(rightChild.get_child(const_index<1>()).get_index(), 1);

    auto twoChildren = n(65)(n(82), n(21));
    QCOMPARE(twoChildren.subtree_size(), 3);
    QCOMPARE(twoChildren.subtree_arity(), 2);
    QCOMPARE(twoChildren.get_value(), 65);
    QCOMPARE(twoChildren.get_child(const_index<0>()).get_value(), 82);
    QCOMPARE(twoChildren.get_child(const_index<1>()).get_value(), 21);
    QCOMPARE(twoChildren.get_child(const_index<0>()).get_index(), 0);
    QCOMPARE(twoChildren.get_child(const_index<1>()).get_index(), 1);

    auto threeChildren = n(45)(n(39), n(71), n(18));
    QCOMPARE(threeChildren.subtree_size(), 4);
    QCOMPARE(threeChildren.subtree_arity(), 3);
    QCOMPARE(threeChildren.get_value(), 45);
    QCOMPARE(threeChildren.get_child(const_index<0>()).get_value(), 39);
    QCOMPARE(threeChildren.get_child(const_index<1>()).get_value(), 71);
    QCOMPARE(threeChildren.get_child(const_index<2>()).get_value(), 18);
    QCOMPARE(threeChildren.get_child(const_index<0>()).get_index(), 0);
    QCOMPARE(threeChildren.get_child(const_index<1>()).get_index(), 1);
    QCOMPARE(threeChildren.get_child(const_index<2>()).get_index(), 2);

    auto fourChildren = n(26)(n(20), n(41), n(15), n(76));
    QCOMPARE(fourChildren.subtree_size(), 5);
    QCOMPARE(fourChildren.subtree_arity(), 4);
    QCOMPARE(fourChildren.get_value(), 26);
    QCOMPARE(fourChildren.get_child(const_index<0>()).get_value(), 20);
    QCOMPARE(fourChildren.get_child(const_index<1>()).get_value(), 41);
    QCOMPARE(fourChildren.get_child(const_index<2>()).get_value(), 15);
    QCOMPARE(fourChildren.get_child(const_index<3>()).get_value(), 76);
    QCOMPARE(fourChildren.get_child(const_index<0>()).get_index(), 0);
    QCOMPARE(fourChildren.get_child(const_index<1>()).get_index(), 1);
    QCOMPARE(fourChildren.get_child(const_index<2>()).get_index(), 2);
    QCOMPARE(fourChildren.get_child(const_index<3>()).get_index(), 3);

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
    QCOMPARE(composedTree.subtree_size(), 29);
    QCOMPARE(composedTree.subtree_arity(), 5);
    QCOMPARE(composedTree.get_value(), 45);
    QCOMPARE(composedTree.get_child(const_index<0>()).get_value(), 97);
    QCOMPARE(
        composedTree.get_child(const_index<0>()).get_child(const_index<0>()).get_value(),
        65);
    QCOMPARE(composedTree.get_child(const_index<0>()).get_value(), 97);
    QCOMPARE(
        composedTree.get_child(const_index<2>()).get_child(const_index<3>()).get_child(const_index<1>()).get_value(),
        41);
    QCOMPARE(composedTree.get_child(const_index<1>()).get_value(), 26);
    QCOMPARE(composedTree.get_child(const_index<2>()).get_value(), 24);
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
    QCOMPARE(tree.subtree_size(), 25);
    QCOMPARE(tree.subtree_arity(), 12);
    QCOMPARE(tree.get_value(), 50);
    QCOMPARE(tree.get_child(const_index<0>()).get_value(), 8001);
    QCOMPARE(tree.get_child(const_index<0>()).get_child(const_index<1>()).subtree_size(), 9);
    QCOMPARE(tree.get_child(const_index<0>()).get_child(const_index<1>()).subtree_arity(), 1);
    QCOMPARE(
        tree.get_child(const_index<0>()).get_child(const_index<7>()).get_child(const_index<2>()).get_value(),
        98235);
    QCOMPARE(tree.get_child(const_index<0>()).get_child(const_index<6>()).get_index(), 6);
    QCOMPARE(tree.get_child(const_index<0>()).get_child(const_index<4>()).get_index(), 4);
    QCOMPARE(tree.get_child(const_index<0>()).get_child(const_index<14>()).get_index(), 14);
    QCOMPARE(tree.get_child(const_index<0>()).get_child(const_index<7>()).subtree_size(), 4);
    QCOMPARE(tree.get_child(const_index<0>()).get_child(const_index<7>()).subtree_arity(), 3);
    QCOMPARE(tree.get_child(const_index<0>()).get_child(const_index<7>()).get_index(), 7);
}

void StructNodeTest::emtyNodes() {
    auto tree
        = n(1)(
            n(2),
            n(),
            n(3)(
                n(4),
                n(5),
                n(6),
                n(),
                n(),
                n(),
                n(7)),
            n(),
            n(8)(
                n(),
                n(),
                n(),
                n(),
                n(),
                n(),
                n(),
                n(),
                n(),
                n(),
                n(),
                n(),
                n(9)));
    QCOMPARE(tree.subtree_size(), 9);
    QCOMPARE(tree.subtree_arity(), 4);
    QCOMPARE(tree.children(), 3);
    QCOMPARE(tree.children_all(), 5);
}

QTEST_MAIN(StructNodeTest);
#include "StructNodeTest.moc"
