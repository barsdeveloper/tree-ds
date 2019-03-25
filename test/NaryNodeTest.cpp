#include <QtTest/QtTest>

#include <TreeDS/tree>

#include "Types.hpp"

using namespace std;
using namespace md;

/*
 * Explicit template instantiation of the class being tested. Without this instantiation, the compiler will not generate
 * the code for all the methods. Those methods will NOT appear as NOT covered by tests (because they will be not
 * recognized as lines of code) giving the false information fo greater coverage percentage.
 */
template class md::nary_node<Target>;

class NaryNodeTest : public QObject {

    Q_OBJECT

    private slots:
    void defaultConstructed();
    void constructFromStructNode();
    void emplacedFromStructNode();
    void moveConstructed();
    void constructFromBinaryNode();
    void equalityWithStructNode();
    void argumentImplicitConvertion();
    void positionalChildGetter();
};

void NaryNodeTest::defaultConstructed() {
    nary_node<int> node;
    nary_node<int> node2;

    QCOMPARE(node.get_parent(), nullptr);
    QCOMPARE(node.get_prev_sibling(), nullptr);
    QCOMPARE(node.get_next_sibling(), nullptr);
    QCOMPARE(node.get_first_child(), nullptr);
    QCOMPARE(node.get_last_child(), nullptr);
    QCOMPARE(node.get_following_siblings(), 0);

    QVERIFY(node.is_root());
    QVERIFY(!node.is_unique_child());
    QVERIFY(!node.is_first_child());
    QVERIFY(!node.is_last_child());

    QVERIFY(node == node2);
    QVERIFY(!(node != node2));
    QVERIFY(node2 == node);
    QVERIFY(!(node2 != node));
}

void NaryNodeTest::constructFromStructNode() {
    auto structNode
        = n(Target("a"))(
            n(Target("b")),
            n(Target("c")));

    nary_node<Target> node(structNode);
    auto& first = *node.get_first_child();
    auto& last  = *node.get_last_child();

    QVERIFY(!node.is_first_child());
    QVERIFY(!node.is_last_child());
    QVERIFY(!node.is_unique_child());
    QCOMPARE(node.get_parent(), nullptr);
    QCOMPARE(node.get_prev_sibling(), nullptr);
    QCOMPARE(node.get_next_sibling(), nullptr);
    QCOMPARE(node.get_following_siblings(), 0);

    QVERIFY(first.is_first_child());
    QVERIFY(!first.is_last_child());
    QVERIFY(!first.is_unique_child());
    QCOMPARE(first.get_parent(), &node);
    QCOMPARE(first.get_prev_sibling(), nullptr);
    QCOMPARE(first.get_next_sibling(), &last);
    QCOMPARE(first.get_following_siblings(), 1);

    QVERIFY(!last.is_first_child());
    QVERIFY(last.is_last_child());
    QVERIFY(!last.is_unique_child());
    QCOMPARE(last.get_parent(), &node);
    QCOMPARE(last.get_prev_sibling(), &first);
    QCOMPARE(last.get_next_sibling(), nullptr);
    QCOMPARE(last.get_following_siblings(), 0);

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

    // move test
    nary_node<Target> newNode(move(node));

    QCOMPARE(*newNode.get_first_child(), first);
    QCOMPARE(*newNode.get_last_child(), last);
    QCOMPARE(node.get_first_child(), nullptr);
    QCOMPARE(node.get_last_child(), nullptr);
    QCOMPARE(node.get_prev_sibling(), nullptr);
    QCOMPARE(node.get_next_sibling(), nullptr);
    QCOMPARE(first.get_parent(), &newNode);
    QCOMPARE(last.get_parent(), &newNode);
    QVERIFY(newNode != node);
}

void NaryNodeTest::emplacedFromStructNode() {
    auto emplacingStructNode
        = n(1, 1)(
            n(2, 1)(
                n(3, 1),
                n(3, 2),
                n(3, 3),
                n(3, 4)),
            n(2, 2)(
                n(4, 1)(
                    n(5, 1),
                    n(5, 2))));
    auto structNode
        = n(Foo(1, 1))(
            n(Foo(2, 1))(
                n(Foo(3, 1)),
                n(Foo(3, 2)),
                n(Foo(3, 3)),
                n(Foo(3, 4))),
            n(Foo(2, 2))(
                n(Foo(4, 1))(
                    n(Foo(5, 1)),
                    n(Foo(5, 2)))));
    nary_node<Foo> node(emplacingStructNode);

    auto& n21 = *node.get_first_child();
    auto& n32 = *n21.get_child(1);
    auto& n52
        = *node.get_child(1)   // 2, 2
               ->get_child(0)  // 4, 1
               ->get_child(1); // 5, 2

    QCOMPARE(n21.get_parent(), &node);
    QCOMPARE(n21.get_following_siblings(), 1);
    QCOMPARE(n32.get_parent(), &n21);
    QCOMPARE(n32.get_following_siblings(), 2);
    QCOMPARE(n52.get_following_siblings(), 0);
    QVERIFY(
        node != n21
        && node != n32
        && node != n52
        && n21 != n32
        && n32 != n52
        && n21 != n52);

    QCOMPARE(node, structNode);
}

void NaryNodeTest::moveConstructed() {
    nary_node<int> moved(
        n(1)(
            n(2),
            n(3)(
                n(6)),
            n(4),
            n(5)));

    auto n2 = moved.get_child(0);
    auto n3 = moved.get_child(1);
    auto n4 = moved.get_child(2);
    auto n5 = moved.get_child(3);

    nary_node<int> target(std::move(moved));

    QCOMPARE(moved.get_first_child(), nullptr);
    QCOMPARE(moved.get_last_child(), nullptr);
    QCOMPARE(moved.get_prev_sibling(), nullptr);
    QCOMPARE(moved.get_next_sibling(), nullptr);
    QCOMPARE(moved.get_following_siblings(), 0);

    QCOMPARE(target.get_first_child(), n2);
    QCOMPARE(target.get_child(1), n3);
    QCOMPARE(target.get_child(2), n4);
    QCOMPARE(target.get_last_child(), n5);
    QCOMPARE(target.get_prev_sibling(), nullptr);
    QCOMPARE(target.get_next_sibling(), nullptr);

    QCOMPARE(n2->get_parent(), &target);
    QCOMPARE(n2->get_prev_sibling(), nullptr);
    QCOMPARE(n2->get_next_sibling(), n3);
    QCOMPARE(n2->get_following_siblings(), 3);

    QCOMPARE(n3->get_parent(), &target);
    QCOMPARE(n3->get_prev_sibling(), n2);
    QCOMPARE(n3->get_next_sibling(), n4);
    QCOMPARE(n3->get_following_siblings(), 2);

    QCOMPARE(n4->get_parent(), &target);
    QCOMPARE(n4->get_prev_sibling(), n3);
    QCOMPARE(n4->get_next_sibling(), n5);
    QCOMPARE(n4->get_following_siblings(), 1);

    QCOMPARE(n5->get_parent(), &target);
    QCOMPARE(n5->get_prev_sibling(), n4);
    QCOMPARE(n5->get_next_sibling(), nullptr);
    QCOMPARE(n5->get_following_siblings(), 0);
}

void NaryNodeTest::constructFromBinaryNode() {
    binary_node<int> binary(
        n(0)(
            n(1)(
                n(2),
                n(3)),
            n(4)(
                n(5))));
    nary_node<int> nary(binary);

    QVERIFY(nary == binary);
    QVERIFY(binary == nary);
    QVERIFY(!(nary != binary));
    QVERIFY(!(binary != nary));

    const nary_node<int>* n1 = nary.get_child(0);
    const nary_node<int>* n2 = nary.get_child(1);

    QCOMPARE(n1->get_parent(), &nary);
    QCOMPARE(n2->get_parent(), &nary);

    binary_node<int> bin1(
        n(0)(
            n(1)(
                n(2),
                n(3)),
            n(4)));

    QVERIFY(nary != bin1);
    QVERIFY(bin1 != nary);

    binary_node<int> bin2(
        n(0)(
            n(1)(
                n(2),
                n(3)),
            n(100)(
                n(5))));

    QVERIFY(nary != bin2);
    QVERIFY(bin2 != nary);
    QVERIFY(bin1 != bin2);
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
    const nary_node<string>* k
        = (&node)
              ->get_child(0)  // b
              ->get_child(1)  // e
              ->get_child(0); // k
    QCOMPARE(
        k
            ->get_child(3) // p
            ->get_value(),
        "p");
    QCOMPARE(k->get_following_siblings(), 1);
    QCOMPARE(k->get_first_child()->get_following_siblings(), 3);
    QCOMPARE(k->get_child(1)->get_following_siblings(), 2);
    QCOMPARE(k->get_child(2)->get_following_siblings(), 1);
    QCOMPARE(k->get_child(3)->get_following_siblings(), 0);
}

QTEST_MAIN(NaryNodeTest);
#include "NaryNodeTest.moc"
