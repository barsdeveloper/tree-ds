#include <QtTest/QtTest>

#include <TreeDS/tree>

#include "Types.hpp"

using namespace std;
using namespace ds;

/*
 * Explicit template instantiation of the class being tested. Without this instantiation, the compiler will not generate
 * the code for all the methods. Those methods will NOT appear as NOT covered by tests (because they will be not
 * recognized as lines of code) giving the false information fo greater coverage percentage.
 */
template class ds::nary_node<Target>;

class NaryNodeTest : public QObject {

    Q_OBJECT

    private slots:
    void defaultConstructed();
    void constructFromStructNode();
    void equalityWithStructNode();
    void argumentImplicitConvertion();
    void positionalChildGetter();
};

void NaryNodeTest::defaultConstructed() {
    nary_node<int> node;
    nary_node<int> node2;

    QVERIFY(node == node2);
    QVERIFY(!(node != node2));
    QVERIFY(node2 == node);
    QVERIFY(!(node2 != node));

    QCOMPARE(node.get_parent(), nullptr);
    QCOMPARE(node.get_first_child(), nullptr);
    QCOMPARE(node.get_last_child(), nullptr);

    QVERIFY(node.is_root());
    QVERIFY(!node.is_unique_child());
    QVERIFY(!node.is_first_child());
    QVERIFY(!node.is_last_child());
}

void NaryNodeTest::constructFromStructNode() {

    auto structNode = n(Target("a"))(
        n(Target("b")),
        n(Target("c")));

    nary_node<Target> node(structNode);
    auto& first = *node.get_first_child();
    auto& last  = *node.get_last_child();

    QVERIFY(node == structNode);
    QVERIFY(structNode == node);
    QVERIFY(!(node != structNode));
    QVERIFY(!(structNode != node));

    QVERIFY(node != first);
    QVERIFY(first != node);
    QVERIFY(node != last);
    QVERIFY(last != node);
    QVERIFY(first != last);
    QVERIFY(last != first);

    QVERIFY(!node.is_first_child());
    QVERIFY(!node.is_last_child());
    QVERIFY(!node.is_unique_child());
    QCOMPARE(node.get_prev_sibling(), nullptr);
    QCOMPARE(node.get_next_sibling(), nullptr);

    QVERIFY(first.is_first_child());
    QVERIFY(!first.is_last_child());
    QVERIFY(!first.is_unique_child());
    QCOMPARE(first.get_prev_sibling(), nullptr);
    QCOMPARE(first.get_next_sibling(), &last);

    QVERIFY(!last.is_first_child());
    QVERIFY(last.is_last_child());
    QVERIFY(!last.is_unique_child());
    QCOMPARE(last.get_prev_sibling(), &first);
    QCOMPARE(last.get_next_sibling(), nullptr);

    // move test
    nary_node<Target> newNode(move(node));

    QVERIFY(newNode != node);
    QCOMPARE(*newNode.get_first_child(), first);
    QCOMPARE(*newNode.get_last_child(), last);
    QCOMPARE(node.get_first_child(), nullptr);
    QCOMPARE(node.get_last_child(), nullptr);
    QCOMPARE(node.get_prev_sibling(), nullptr);
    QCOMPARE(node.get_next_sibling(), nullptr);
}

void NaryNodeTest::equalityWithStructNode() {
    nary_node<string> someChildren(
        n("a")(
            n("b"),
            n("c"),
            n("d"),
            n("e")));
    nary_node<string> firstChild(
        n("a")(
            n("b")));
    nary_node<string> firstWrong(
        n("a")(
            n("#"),
            n("c"),
            n("d"),
            n("e")));
    nary_node<string> secondWrong(
        n("a")(
            n("b"),
            n("#"),
            n("d"),
            n("e")));

    QVERIFY(someChildren == someChildren);
    QVERIFY(!(someChildren != someChildren));
    QVERIFY(someChildren != firstChild);
    QVERIFY(someChildren != firstChild);

    QVERIFY(someChildren == n("a")(n("b"), n("c"), n("d"), n("e")));
    QVERIFY(someChildren != n("a")(n("b"), n("c")(n()), n("d"), n("e")));
    QVERIFY(someChildren != n("a")(n("b"), n("c")(n()(n("#"))), n("d"), n("e")));
    QVERIFY(someChildren != n("a")(n("b"), n("c"), n("d"), n("e"), n("f")));
    QVERIFY(someChildren != n("a")(n("b"), n("c"), n("d"), n("e"), n("f"), n("g")));
    QVERIFY(someChildren != n("a")(n("b"), n("c")(n("#")), n("d"), n("e")));
    QVERIFY(someChildren != n("a")(n("b"), n("c"), n("d")));
    QVERIFY(someChildren != n("#")(n("b"), n("c")));
    QVERIFY(someChildren != n("a")(n("b")));
    QVERIFY(someChildren != n("a"));
    QVERIFY(someChildren != n("a")(n("b"), n("c"), n("d"), n("#")));
    QVERIFY(someChildren != n("a")(n("b"), n("c"), n("#"), n("e")));
    QVERIFY(someChildren != n("a")(n("b"), n("#"), n("d"), n("e")));
    QVERIFY(someChildren != n("a")(n("#"), n("c"), n("d"), n("e")));
    QVERIFY(someChildren != n("#")(n("#"), n("c"), n("d"), n("e")));
    QVERIFY(someChildren != n("a")(n("b")));
    QVERIFY(someChildren != n("a")(n("#"), n("c")));
    QVERIFY(someChildren != n("a")(n(), n("c")));
    QVERIFY(someChildren != n("a"));

    QVERIFY(firstChild == n("a")(n("b")));
    QVERIFY(firstChild != n("a")(n("#")));
    QVERIFY(firstChild != n("a"));
}

void NaryNodeTest::argumentImplicitConvertion() {
    auto from = ConvertibleFrom("someString");
    auto to   = ConvertibleTo("someString");
    nary_node<Target> regular(Target(string("someString")));
    nary_node<Target> constructedFrom(from);
    nary_node<Target> constructedTo(to);
    nary_node<Target> different("diffentString");

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

void NaryNodeTest::positionalChildGetter() {
    nary_node<string> node(
        n("a")(
            n("b")(
                n("d"),
                n("e")(
                    n("k")(
                        n("m"),
                        n("n"),
                        n("o"),
                        n("p")),
                    n("l")),
                n("f"),
                n("g")),
            n("c")(
                n("h"),
                n("i"),
                n("j"))));
    QCOMPARE(
        (&node)
            ->get_child(0)
            ->get_child(1)
            ->get_child(0)
            ->get_child(3)
            ->get_value(),
        "p");
}

QTEST_MAIN(NaryNodeTest);
#include "NaryNodeTest.moc"
