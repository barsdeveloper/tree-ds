#include <QtTest/QtTest>

#include <TreeDS/tree>

#include "Types.hpp"

using namespace std;
using namespace md;

template class md::tree_iterator<nary_tree<int>, pre_order, false>;
template class md::tree_iterator<binary_tree<Bar>, in_order, true>;

class IteratorTest : public QObject {

    Q_OBJECT

    private slots:
    void test1();
    void test2();
};

void IteratorTest::test1() {
    nary_tree<int, post_order> tree;
    decltype(tree)::iterator<post_order> unrelated;
    auto start = tree.begin();
    decltype(tree)::iterator<post_order> related(start);

    QVERIFY(unrelated != related);
    QVERIFY(related != unrelated);
    QVERIFY(!(unrelated == related));
    QVERIFY(!(related == unrelated));

    QVERIFY(start == related);
    QVERIFY(related == start);
    QVERIFY(!(start != related));
    QVERIFY(!(related != start));
}

void IteratorTest::test2() {
    using iterator_t       = binary_tree<Bar>::iterator<in_order>;
    using const_iterator_t = binary_tree<Bar>::const_iterator<in_order>;

    binary_tree<Bar> tree(n(1, 2)(n(2, 3), n(3, 4)));

    // obtained using std::find
    iterator_t it1 = std::find(tree.begin(in_order()), tree.end(in_order()), Bar(1, 2));
    // copy constructed
    iterator_t it2(it1);
    // copy assignment (below)
    iterator_t it3;
    it3 = it1;
    // unmodifiable reference to a pointer
    const iterator_t& const_ref = it1;

    ++it2; // it2 == it1 + 1
    --it3; // it3 == it1 - 1

    // constant iterator copy constructed
    const_iterator_t itc1(it1);
    // contant iterator copy assigned (below)
    const_iterator_t itc2;
    itc2 = it2;

    // 3 iterators, all different each other
    QVERIFY(it1 != it2 && it2 != it3 && it3 != it2);

    // Compare agains value
    QCOMPARE(*it1, Bar(1, 2));
    QCOMPARE(*it2, Bar(3, 4));
    QCOMPARE(*it3, Bar(2, 3));
    QCOMPARE(*itc1, Bar(1, 2));
    QCOMPARE(*itc2, Bar(3, 4));

    // Indirect method call (through iterator)
    QCOMPARE(it1->method(), "mutable");
    QCOMPARE(itc1->method(), "constant");
    QCOMPARE(const_ref->method(), "constant");
    QCOMPARE((*it1).method(), "mutable");
    QCOMPARE((*itc1).method(), "constant");
    QCOMPARE((*const_ref).method(), "constant");

    // Direct method call (thorugh iterator->node->value->method)
    QCOMPARE(it1.get_node()->get_value().method(), "mutable");
    QCOMPARE(itc1.get_node()->get_value().method(), "constant");
    QCOMPARE(const_ref.get_node()->get_value().method(), "constant");

    // Iterator equals constant itself
    QVERIFY(it1 == itc1);
    QVERIFY(itc1 == it1);
    QVERIFY(!(it1 != itc1));
    QVERIFY(!(itc1 != it1));

    // Different iterators are not equal
    QVERIFY(it1 != it2);
    QVERIFY(it2 != it1);
    QVERIFY(!(it1 == it2));
    QVERIFY(!(it2 == it1));

    // Iterator equals constant itself (using it2)
    QVERIFY(it2 == itc2);
    QVERIFY(itc2 == it2);
    QVERIFY(!(it2 != itc2));
    QVERIFY(!(itc2 != it2));

    // Different constant iterators are not equal
    QVERIFY(itc1 != itc2);
    QVERIFY(itc2 != itc1);
    QVERIFY(!(itc1 == itc2));
    QVERIFY(!(itc2 == itc1));
}

QTEST_MAIN(IteratorTest)
#include "IteratorTest.moc"
