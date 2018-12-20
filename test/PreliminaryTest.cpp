#include <QtTest/QtTest>

#include <TreeDS/tree>

#include "Types.hpp"

using namespace std;
using namespace md;

class PreliminaryTest : public QObject {

    Q_OBJECT

    private slots:
    void objectCreation();
};

void PreliminaryTest::objectCreation() {

    binary_tree<Foo> t;

    // The tree is empty
    QCOMPARE(t.size(), 0);
    QVERIFY(t.empty());

    // Iterator point to the end
    QVERIFY(t.begin() == t.end());
    QVERIFY(t.cbegin() == t.cend());
    QVERIFY(t.rbegin() == t.rend());
    QVERIFY(t.crbegin() == t.crend());

    // Iterator to const_iterator equality compatibility
    QVERIFY(t.cbegin() == t.end());
    QVERIFY(t.crbegin() == t.rend());

    // construct an element
    t.emplace(t.begin(), 67, 93);
    QCOMPARE(t, n(Foo(67, 93)));

    // The tree has 1 element
    QCOMPARE(t.size(), 1);
    QVERIFY(!t.empty());

    // All iterators begin from the unique element
    QCOMPARE(*t.begin(), Foo(67, 93));
    QCOMPARE(*t.cbegin(), Foo(67, 93));
    QCOMPARE(*t.rbegin(), Foo(67, 93));
    QCOMPARE(*t.crbegin(), Foo(67, 93));

    // Iterator no more point to the end
    QVERIFY(t.begin() != t.end());
    QVERIFY(t.cbegin() != t.cend());
    QVERIFY(t.rbegin() != t.rend());
    QVERIFY(t.crbegin() != t.crend());

    // End is one after begin
    QVERIFY(++t.begin() == t.end());
    QVERIFY(++t.cbegin() == t.cend());
    QVERIFY(++t.rbegin() == t.rend());
    QVERIFY(++t.crbegin() == t.crend());

    // Begin is one before end
    QVERIFY(t.begin() == --t.end());
    QVERIFY(t.cbegin() == --t.cend());
    QVERIFY(t.rbegin() == --t.rend());
    QVERIFY(t.crbegin() == --t.crend());

    // Delete all elements
    t.clear();

    // The tree is again empty
    QCOMPARE(t.size(), 0);
    QVERIFY(t.empty());

    // Iterators point again at the end
    QVERIFY(t.begin() == t.end());
    QVERIFY(t.cbegin() == t.cend());
    QVERIFY(t.rbegin() == t.rend());
    QVERIFY(t.crbegin() == t.crend());

    // Create by insert
    t.insert(t.begin(), Foo(45, 900));
    QCOMPARE(*t.begin(), Foo(45, 900));
    QCOMPARE(t, n(Foo(45, 900)));

    // The tree has 1 element again
    QCOMPARE(t.size(), 1);
    QVERIFY(!t.empty());
}

QTEST_MAIN(PreliminaryTest);
#include "PreliminaryTest.moc"
