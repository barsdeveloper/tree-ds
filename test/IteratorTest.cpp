#include <QtTest/QtTest>

#include <TreeDS/tree>

#include "Types.hpp"

using namespace std;
using namespace ds;

class IteratorTest : public QObject {

    Q_OBJECT

    private slots:
    void test1();
    void test2();
};

void IteratorTest::test1() {
    nary_tree<int, post_order> tree;
    decltype(tree)::iterator<post_order> unrelated;
    decltype(tree)::iterator<post_order> related(tree);
    auto start = tree.begin();

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
    iterator_t it1 = std::find(tree.begin<in_order>(), tree.end<in_order>(), Bar(1, 2));
    iterator_t it2(it1);  // copy construction
    iterator_t it3 = it2; // copy assignment
    ++it2;
    --it3;
    const_iterator_t itc1(it1);
    const_iterator_t itc2;
    itc2 = it2;

    QCOMPARE(*it1, Bar(1, 2));
    QCOMPARE(*it2, Bar(3, 4));
    QCOMPARE(*it3, Bar(2, 3));
    QCOMPARE(*itc1, Bar(1, 2));
    QCOMPARE(*itc2, Bar(3, 4));
    QCOMPARE(it1->method(), "mutable");
    QCOMPARE(itc1->method(), "constant");
    QVERIFY(it1 != it2 && it2 != it3 && it3 != it2);

    QVERIFY(it1 == itc1);
    QVERIFY(itc1 == it1);
    QVERIFY(!(it1 != itc1));
    QVERIFY(!(itc1 != it1));

    QVERIFY(it1 != it2);
    QVERIFY(it2 != it1);
    QVERIFY(!(it1 == it2));
    QVERIFY(!(it2 == it1));

    QVERIFY(it2 == itc2);
    QVERIFY(itc2 == it2);
    QVERIFY(!(it2 != itc2));
    QVERIFY(!(itc2 != it2));

    QVERIFY(itc1 != itc2);
    QVERIFY(itc2 != itc1);
    QVERIFY(!(itc1 == itc2));
    QVERIFY(!(itc2 == itc1));
}

QTEST_MAIN(IteratorTest)
#include "IteratorTest.moc"
