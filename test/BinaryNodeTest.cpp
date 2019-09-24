#include <QtTest/QtTest>

#include <TreeDS/tree>

#include "Types.hpp"

using namespace std;
using namespace md;

/*
 * Explicit template instantiation of the class being tested. Without this instantiation, the compiler will not generate
 * the code for all the methods. Those methods will NOT appear as NOT covered by tests (because they will be not
 * recognized as lines of code) giving the false information of greater coverage percentage.
 */
template class md::binary_node<Target>;

class BinaryNodeTest : public QObject {

    Q_OBJECT

    private slots:
    void defaultConstructed();
    void constructFromStructNode();
    void equalityWithStructNode();
    void argumentImplicitConvertion();
    void nodeTransform();
};

void BinaryNodeTest::defaultConstructed() {
    binary_node<int> node;
    binary_node<int> node2;

    QVERIFY(node == node2);
    QVERIFY(!(node != node2));
    QVERIFY(node2 == node);
    QVERIFY(!(node2 != node));

    QCOMPARE(node.get_parent(), nullptr);
    QCOMPARE(node.get_prev_sibling(), nullptr);
    QCOMPARE(node.get_next_sibling(), nullptr);
    QCOMPARE(node.get_first_child(), nullptr);
    QCOMPARE(node.get_last_child(), nullptr);
    QCOMPARE(node.get_left_child(), nullptr);
    QCOMPARE(node.get_right_child(), nullptr);
    QCOMPARE(node.children(), 0u);
    QCOMPARE(node.following_siblings(), 0u);

    QVERIFY(node.is_root());
    QVERIFY(!node.is_unique_child());
    QVERIFY(!node.is_first_child());
    QVERIFY(!node.is_last_child());
    QVERIFY(!node.is_left_child());
    QVERIFY(!node.is_right_child());
}

void BinaryNodeTest::constructFromStructNode() {
    auto structNode = n(Target("a"))(
        n(Target("b")),
        n(Target("c")));
    binary_node<Target> node(structNode);
    auto& left  = *node.get_left_child();
    auto& right = *node.get_right_child();

    QVERIFY(node == structNode);
    QVERIFY(structNode == node);
    QVERIFY(!(node != structNode));
    QVERIFY(!(structNode != node));

    QVERIFY(node != left);
    QVERIFY(left != node);
    QVERIFY(node != right);
    QVERIFY(right != node);
    QVERIFY(left != right);
    QVERIFY(right != left);

    QVERIFY(!node.is_left_child());
    QVERIFY(!node.is_right_child());
    QVERIFY(!node.is_first_child());
    QVERIFY(!node.is_last_child());
    QVERIFY(!node.is_unique_child());
    QCOMPARE(node.get_prev_sibling(), nullptr);
    QCOMPARE(node.get_next_sibling(), nullptr);
    QCOMPARE(node.children(), 2);
    QCOMPARE(node.following_siblings(), 0u);

    QVERIFY(left.is_left_child());
    QVERIFY(!left.is_right_child());
    QVERIFY(left.is_first_child());
    QVERIFY(!left.is_last_child());
    QVERIFY(!left.is_unique_child());
    QCOMPARE(left.get_prev_sibling(), nullptr);
    QCOMPARE(left.get_next_sibling(), &right);
    QCOMPARE(left.children(), 0);
    QCOMPARE(left.following_siblings(), 1u);

    QVERIFY(!right.is_left_child());
    QVERIFY(right.is_right_child());
    QVERIFY(!right.is_first_child());
    QVERIFY(right.is_last_child());
    QVERIFY(!right.is_unique_child());
    QCOMPARE(right.get_prev_sibling(), &left);
    QCOMPARE(right.get_next_sibling(), nullptr);
    QCOMPARE(right.children(), 0u);
    QCOMPARE(right.following_siblings(), 0u);

    // Move test
    binary_node<Target> newNode(move(node));

    QVERIFY(newNode != node);
    QCOMPARE(newNode.get_left_child(), &left);
    QCOMPARE(newNode.get_right_child(), &right);
    QCOMPARE(node.get_left_child(), nullptr);
    QCOMPARE(node.get_right_child(), nullptr);
}

void BinaryNodeTest::equalityWithStructNode() {
    binary_node<string> bothChild(
        n("a")(
            n("b"),
            n("c")));
    binary_node<string> leftChild(
        n("a")(
            n("b")));
    binary_node<string> rightChild(
        n("a")(
            n(),
            n("b")));
    binary_node<string> leftWrong(
        n("a")(
            n("#"),
            n("c")));
    binary_node<string> rightWrong(
        n("a")(
            n("b"),
            n("#")));

    QVERIFY(bothChild == bothChild);
    QVERIFY(!(bothChild != bothChild));
    QVERIFY(bothChild != leftChild);
    QVERIFY(bothChild != rightChild);
    QVERIFY(bothChild != leftWrong);
    QVERIFY(bothChild != rightWrong);

    QVERIFY(bothChild == n("a")(n("b"), n("c")));
    QVERIFY(bothChild == n("a")(n("b")(n()), n("c")));
    QVERIFY(bothChild == n("a")(n("b"), n("c"), n(), n()));
    QVERIFY(bothChild != n("a")(n("b"), n("c"), n("d")));
    QVERIFY(bothChild != n("#")(n("b"), n("c")));
    QVERIFY(bothChild != n("a")(n("b"), n("#")));
    QVERIFY(bothChild != n("a")(n("b"), n()));
    QVERIFY(bothChild != n("a")(n("b")));
    QVERIFY(bothChild != n("a")(n("#"), n("c")));
    QVERIFY(bothChild != n("a")(n(), n("c")));
    QVERIFY(bothChild != n("a"));

    QVERIFY(leftChild == n("a")(n("b")));
    QVERIFY(leftChild != n("a")(n("#")));
    QVERIFY(leftChild != n("a"));

    QVERIFY(rightChild == n("a")(n(), n("b")));
    QVERIFY(rightChild != n("a")(n(), n("#")));
    QVERIFY(rightChild != n("a"));
}

void BinaryNodeTest::argumentImplicitConvertion() {
    auto from = ConvertibleFrom("someString");
    auto to   = ConvertibleTo("someString");
    binary_node<Target> regular(Target(string("someString")));
    binary_node<Target> constructedFrom(from);
    binary_node<Target> constructedTo(to);
    binary_node<Target> different("diffentString");

    QCOMPARE("someString", constructedFrom.get_value().value);
    QCOMPARE("someString", constructedTo.get_value().value);
    QCOMPARE(regular, n(Target("someString")));
    QCOMPARE(regular, n(string("someString")));
    QCOMPARE(regular, constructedFrom);
    QCOMPARE(regular, constructedTo);

    // Various operator== test
    QVERIFY(regular != different);
    QVERIFY(regular == constructedFrom);
    QVERIFY(!(regular != constructedFrom));
    QVERIFY(constructedFrom == regular);
    QVERIFY(!(constructedFrom != regular));
}

void BinaryNodeTest::nodeTransform() {
    binary_node<int> node(
        n(-1)(
            n(),
            n(-2)(
                n(-3)(
                    n(-5)),
                n(-4)(
                    n(),
                    n(-6)))));
}

QTEST_MAIN(BinaryNodeTest);
#include "BinaryNodeTest.moc"
