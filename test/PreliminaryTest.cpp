#include <QtTest/QtTest>
#include <TreeDS/tree.hpp>

using namespace std;
using namespace ds;

struct Dummy {
    int a;
    int b;
};

Q_DECLARE_METATYPE(Dummy);
Q_DECLARE_METATYPE(tree<Dummy>);

class PreliminaryTest : public QObject {

    Q_OBJECT;

private slots:
    void objectCreation() {
        tree<Dummy> t;
        QCOMPARE(t.size(), 0);
        QVERIFY(t.empty());
        QVERIFY(t.begin() == t.end());

        t.emplace(t.begin(), 67, 93);
        QCOMPARE(t.size(), 1);
        QVERIFY(!t.empty());
        QVERIFY(t.begin() != t.end());
        }
};

QTEST_MAIN(PreliminaryTest);
#include "PreliminaryTest.moc"