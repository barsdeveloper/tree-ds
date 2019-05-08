#include <QtTest/QtTest>

#include <TreeDS/match>
#include <TreeDS/tree>

#include "Types.hpp"

using namespace std;
using namespace md;

class MatcherTest : public QObject {

    Q_OBJECT

    binary_tree<int> tree {
        n(1)(
            n(2),
            n(3))};

    private slots:
    void patternConstruction();
};

void MatcherTest::patternConstruction() {
    pattern simple(one(1));
    QVERIFY(simple.match(tree));
}

QTEST_MAIN(MatcherTest);
#include "MatcherTest.moc"
