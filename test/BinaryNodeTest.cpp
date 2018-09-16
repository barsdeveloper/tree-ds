#include <QtTest/QtTest>

#include <TreeDS/tree.hpp>

#include "Types.hpp"

using namespace std;
using namespace ds;

/*
 * Explicit template instantiation of the class being tested. Without this instantiation, the compiler will not generate
 * the code for all the methods. Those methods will NOT appear as NOT covered by tests (because they will be not
 * recognized as lines of code) giving the false information fo greater coverage percentage.
 */
template class ds::binary_node<Target>;

class BinaryNodeTest : public QObject {

    Q_OBJECT;

    private slots:
    void defaultConstructed() {
        binary_node<int> node;
        binary_node<int> node2;

        QVERIFY(node == node2);
        QVERIFY(!(node != node2));
        QVERIFY(node2 == node);
        QVERIFY(!(node2 != node));

        QCOMPARE(nullptr, node.get_parent());
        QCOMPARE(nullptr, node.first_child());
        QCOMPARE(nullptr, node.last_child());
        QCOMPARE(nullptr, node.get_left());
        QCOMPARE(nullptr, node.get_right());

        QVERIFY(!node.is_unique_child());
        QVERIFY(!node.is_first_child());
        QVERIFY(!node.is_last_child());
        QVERIFY(!node.is_left_child());
        QVERIFY(!node.is_right_child());
    }

    void constructFromStructNode() {
        // clang-format off
        auto structNode = 
            n(Target("a"))(
                n(Target("b")),
                n(Target("c"))
            );
        // clang-format on
        binary_node<Target> node(structNode);

        QVERIFY(node == structNode);
        QVERIFY(structNode == node);
        QVERIFY(!(node != structNode));
        QVERIFY(!(structNode != node));
    }

    void equalityWithStructNode() {
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

        QVERIFY(bothChild == n("a")(n("b"), n("c")));
        QVERIFY(bothChild != n("a")(n("b"), n("c"), n("d")));
        QVERIFY(bothChild != n("x")(n("b"), n("c")));
        QVERIFY(bothChild != n("a")(n("b"), n("x")));
        QVERIFY(bothChild != n("a")(n("b"), n()));
        QVERIFY(bothChild != n("a")(n("b")));
        QVERIFY(bothChild != n("a")(n("x"), n("c")));
        QVERIFY(bothChild != n("a")(n(), n("c")));
        QVERIFY(bothChild != n("a"));

        QVERIFY(leftChild == n("a")(n("b")));
        QVERIFY(leftChild != n("a")(n("x")));
        QVERIFY(leftChild != n("a"));

        QVERIFY(rightChild == n("a")(n(), n("b")));
        QVERIFY(rightChild != n("a")(n(), n("x")));
        QVERIFY(rightChild != n("a"));
    }

    void argumentImplicitConvertion() {
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

        // various operator== test
        QVERIFY(regular != different);
        QVERIFY(regular == constructedFrom);
        QVERIFY(!(regular != constructedFrom));
        QVERIFY(constructedFrom == regular);
        QVERIFY(!(constructedFrom != regular));
    }
};

QTEST_MAIN(BinaryNodeTest);
#include "BinaryNodeTest.moc"