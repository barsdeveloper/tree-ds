#include <QtTest/QtTest>
#include <TreeDS/tree.hpp>
#include "Types.hpp"

using namespace std;
using namespace ds;

Q_DECLARE_METATYPE(tree<Foo>);

class PreliminaryTest : public QObject {

    Q_OBJECT;

private slots:
    void objectCreation() {

        tree<Foo> t;

        // the tree is empty
        QCOMPARE(t.size(), 0);
        QVERIFY(t.empty());

        // iterator point to the end
        QVERIFY(t.begin() == t.end());
        QVERIFY(t.cbegin() == t.cend());
        QVERIFY(t.rbegin() == t.rend());
        QVERIFY(t.crbegin() == t.crend());

        // construct an element
        t.emplace(t.begin(), 67, 93);

        // the tree has 1 element
        QCOMPARE(t.size(), 1);
        QVERIFY(!t.empty());

        // iterator no more point to the end
        QVERIFY(t.begin() != t.end());
        QVERIFY(t.cbegin() != t.cend());
        QVERIFY(t.rbegin() != t.rend());
        QVERIFY(t.crbegin() != t.crend());

        // end is one after begin
        QVERIFY(++t.begin() == t.end());
        QVERIFY(++t.cbegin() == t.cend());
        QVERIFY(++t.rbegin() == t.rend());
        QVERIFY(++t.crbegin() == t.crend());

        // begin is one before end
        QVERIFY(t.begin() == --t.end());
        QVERIFY(t.cbegin() == --t.cend());
        QVERIFY(t.rbegin() == --t.rend());
        QVERIFY(t.crbegin() == --t.crend());

        // delete all elements
        t.clear();

        // the tree is again empty
        QCOMPARE(t.size(), 0);
        QVERIFY(t.empty());

        // the iterators point again at the end
        QVERIFY(t.begin() == t.end());
        QVERIFY(t.cbegin() == t.cend());
        QVERIFY(t.rbegin() == t.rend());
        QVERIFY(t.crbegin() == t.crend());
    }
};

QTEST_MAIN(PreliminaryTest);
#include "PreliminaryTest.moc"
