#include <QtTest/QtTest>
#include <functional>
#include <list>

#include <TreeDS/tree>

using namespace std;
using namespace md;

// Test statefull iterators in order to check the robustness to keep a coherent internal state
class BreadthFirstTest : public QObject {

    Q_OBJECT
    private:
    // Initial tree to be modified
    static const nary_tree<int> tree;

    private slots:
    // Traverse in an unusual way: jump forward and backward
    void backAndForth();
    // Check wether the iterator is updated consistently (update is required in case of insertion operation)
    void checkUpdateConsistency();
};

const nary_tree<int> BreadthFirstTest::tree(
    n(1)(
        n(2)(
            n(6)(
                n(10),
                n(11)(
                    n(16)),
                n(12)),
            n(7)(
                n(13)(
                    n(17)(
                        n(24)(
                            n(30)(
                                n(37)),
                            n(31)(
                                n(38),
                                n(39)(
                                    n(46),
                                    n(47))),
                            n(32),
                            n(33))),
                    n(18),
                    n(19)(
                        n(25))))),
        n(3)(
            n(8)),
        n(4),
        n(5)(
            n(9)(
                n(14)(
                    n(20),
                    n(21)(
                        n(26),
                        n(27)(
                            n(34)(
                                n(40)(
                                    n(48),
                                    n(49))),
                            n(35)(
                                n(41),
                                n(42),
                                n(43),
                                n(44))))),
                n(15)(
                    n(22)(
                        n(28)(
                            n(36)(
                                n(45)(
                                    n(50)))),
                        n(29)),
                    n(23))))));

void BreadthFirstTest::backAndForth() {
    vector<int> result;
    vector<int> expected(50);
    // populate result with the actual result of tree traverse
    result.assign(tree.cbegin(breadth_first()), tree.cend(breadth_first()));
    // the expected traversal is 1...50
    iota(expected.begin(), expected.end(), 1);

    QCOMPARE(result, expected);

    // Iterators of expected
    auto expectedStart = expected.begin();
    auto expectedEnd   = expected.end();
    auto expectedIt    = expectedStart;

    // Iterators of the actual tree
    auto actualStart = tree.cbegin(breadth_first());
    auto actualEnd   = tree.cend(breadth_first());
    auto actualIt    = actualStart;

    // start jumping forward and backward
    int index = 0;
    while (expectedIt != expectedEnd) { // until the end
        int offset = 0;
        // take care it will reaches the end (totally must add more than subtract)
        switch (index++ % 4) {
        case 0:
            offset = 3;
            break;
        case 1:
            offset = -5;
            break;
        case 2:
            offset = 7;
            break;
        case 3:
            offset = -4;
            break;
        }
        // 3 - 5 + 7 - 4 = 1 ok, it will reach the end
        while (offset != 0 && expectedIt != expectedEnd && actualIt != actualEnd) {
            QVERIFY(*expectedIt == *actualIt);
            if (offset > 0) { // positive offset increments
                if (expectedIt == expectedEnd || actualIt == actualEnd) {
                    break; // if it reached the end, simply end the loop
                }
                --offset;
                ++expectedIt;
                ++actualIt;
            } else { // negative offset decrements
                if (expectedIt == expectedStart || expectedIt == expectedEnd) {
                    break; // if it reached the start, simply end the loop
                }
                ++offset;
                --expectedIt;
                --actualIt;
            }
        }
    }
}

void BreadthFirstTest::checkUpdateConsistency() {
    nary_tree<int> t(BreadthFirstTest::tree);
    auto it24 = find(
        t.begin(breadth_first()),
        t.end(breadth_first()),
        24);
    auto it23 = (--it24)++;
    auto it22 = (--it23)++;
    auto it25 = (++it24)--;
    auto it26 = (++it25)--;

    QCOMPARE(*it22, 22);
    QCOMPARE(*it23, 23);
    QCOMPARE(*it24, 24);
    QCOMPARE(*it25, 25);
    QCOMPARE(*it26, 26);

    // replace node 24 with another subtree
    auto former24 = t.insert(
        it24,
        n(100)(
            n(101)(
                n(108),
                n(109)),
            n(102),
            n(103)(
                n(110)(
                    n(120),
                    n(121)(
                        n(130))),
                n(111)),
            n(104)(
                n(112)(
                    n(122),
                    n(123))),
            n(105)(
                n(113),
                n(114),
                n(115)(
                    n(124),
                    n(125))),
            n(106),
            n(107)(
                n(116),
                n(117)(
                    n(126),
                    n(127),
                    n(128),
                    n(129)),
                n(118),
                n(119))));
    vector<int> newNodes {
        *former24,   // 100
        *++former24, // 25
        *++former24, // 26
        *++former24, // 27
        *++former24, // 28
        *++former24, // 29
        *++former24, // 101
        *++former24, // 102
        *++former24, // 103
        *++former24, // 104
        *++former24, // 105
        *++former24, // 106
        *++former24, // 107
        *++former24, // 34
        *++former24, // 35
        *++former24, // 36
        *++former24, // 108
        *++former24, // 109
        *++former24, // 110
        *++former24, // 111
        *++former24, // 112
        *++former24, // 113
        *++former24, // 114
        *++former24, // 115
        *++former24, // 116
        *++former24, // 117
        *++former24, // 118
        *++former24, // 119
        *++former24, // 40
        *++former24, // 45
    };
    vector<int> expected {100, 25, 26, 27, 28, 29, 101, 102, 103, 104, 105, 106, 107, 34, 35, 36,
                          108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 40, 41};
    QCOMPARE(newNodes, expected);
}

QTEST_MAIN(BreadthFirstTest);
#include "BreadthFirstTest.moc"
