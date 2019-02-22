#include <QtTest/QtTest>

//#include <TreeDS/match>
#include <TreeDS/tree>

using namespace std;
using namespace md;

class MatcherTest : public QObject {

    Q_OBJECT

    private:
    binary_tree<int> binary
        = n(1)(
            n(2)(
                n(4)),
            n(3)(
                n(5),
                n(6)(
                    n(7),
                    n(8))));

    private slots:
    void preliminaryTest();
};

void MatcherTest::preliminaryTest() {
    /*binary_node<int>* target;
    binary_node<int>* ignored;
    auto matcher
        = one(ignored)(
            one(target));
    bool result = matcher.match_tree(this->binary);
    QVERIFY(result);
    QCOMPARE(target, this->binary.get_root()->get_first_child());*/
}

QTEST_MAIN(MatcherTest);
#include "MatcherTest.moc"
