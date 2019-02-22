#include <QtTest/QtTest>
#include <algorithm>
#include <iterator>
#include <vector>

#include <TreeDS/tree>

using namespace std;
using namespace md;

Q_DECLARE_METATYPE(vector<char>);
Q_DECLARE_METATYPE(nary_tree<char>);

class NaryTreeIterationTest : public QObject {

    Q_OBJECT

    private slots:
    void iteration();
    void iteration_data();
};

void NaryTreeIterationTest::iteration() {
    QFETCH(nary_tree<char>, t);
    QFETCH(int, expectedSize);
    QFETCH(int, expectedArity);
    QFETCH(vector<char>, expectedPreOrder);
    QFETCH(vector<char>, expectedPostOrder);
    QFETCH(vector<char>, expectedBreadthFirst);
    int actualSize  = static_cast<int>(t.size());
    int actualArity = static_cast<int>(t.arity());

    // Preliminary test
    QCOMPARE(actualSize, expectedSize);
    QCOMPARE(actualArity, expectedArity);

    /*   ---   Forward order test   ---   */
    vector<char> actualPreOrder;
    vector<char> actualPostOrder;
    vector<char> actualBreadthFirst;

    actualPreOrder.assign(t.begin(pre_order()), t.end(pre_order()));
    actualPostOrder.assign(t.begin(post_order()), t.end(post_order()));
    actualBreadthFirst.assign(t.begin(breadth_first()), t.end(breadth_first()));

    QCOMPARE(actualPreOrder, expectedPreOrder);
    QCOMPARE(actualPostOrder, expectedPostOrder);
    QCOMPARE(actualBreadthFirst, expectedBreadthFirst);

    /*   ---   Backward order test   ---   */
    vector<char> actualNaryReversePreOrder;
    vector<char> actualNaryReversePostOrder;
    vector<char> actualNaryReverseBreadthFirst;

    vector<char> expectedReversePreOrder;
    vector<char> expectedReverseInOrder;
    vector<char> expectedReversePostOrder;
    vector<char> expectedReverseBreadthFirst;

    actualNaryReversePreOrder.assign(t.rbegin(pre_order()), t.rend(pre_order()));
    actualNaryReversePostOrder.assign(t.rbegin(post_order()), t.rend(post_order()));
    actualNaryReverseBreadthFirst.assign(t.rbegin(breadth_first()), t.rend(breadth_first()));

    expectedReversePreOrder.assign(expectedPreOrder.rbegin(), expectedPreOrder.rend());
    expectedReversePostOrder.assign(expectedPostOrder.rbegin(), expectedPostOrder.rend());
    expectedReverseBreadthFirst.assign(expectedBreadthFirst.rbegin(), expectedBreadthFirst.rend());

    QCOMPARE(actualNaryReversePreOrder, expectedReversePreOrder);
    QCOMPARE(actualNaryReversePostOrder, expectedReversePostOrder);
    QCOMPARE(actualNaryReverseBreadthFirst, expectedReverseBreadthFirst);
}

void NaryTreeIterationTest::iteration_data() {
    QTest::addColumn<nary_tree<char>>("t");
    QTest::addColumn<int>("expectedSize");
    QTest::addColumn<int>("expectedArity");
    QTest::addColumn<vector<char>>("expectedPreOrder");
    QTest::addColumn<vector<char>>("expectedPostOrder");
    QTest::addColumn<vector<char>>("expectedBreadthFirst");

    /******************************************************************************************************************/
    QTest::newRow("Bottom wide")
        << nary_tree<char>(
               n('a')(
                   n('b')(
                       n('c')(
                           n('d')(
                               n('e')(
                                   n('f'),
                                   n('g'),
                                   n('h'),
                                   n('i'),
                                   n('j'),
                                   n('k'),
                                   n('l')))))))
        << 12
        << 7
        << vector<char> {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l'}
        << vector<char> {'f', 'g', 'h', 'i', 'j', 'k', 'l', 'e', 'd', 'c', 'b', 'a'}
        << vector<char> {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l'};

    /******************************************************************************************************************/
    QTest::newRow("Triangular")
        << nary_tree<char>(
               n('a')(
                   n('b')(
                       n('i')),
                   n('c'),
                   n('d')(
                       n('j')(
                           n('m'))),
                   n('e'),
                   n('f')(
                       n('k')(
                           n('n')(
                               n('p')))),
                   n('g'),
                   n('h')(
                       n('l')(
                           n('o')(
                               n('q')(
                                   n('r')))))))
        << 18
        << 7
        << vector<char> {'a', 'b', 'i', 'c', 'd', 'j', 'm', 'e', 'f', 'k', 'n', 'p', 'g', 'h', 'l', 'o', 'q', 'r'}
        << vector<char> {'i', 'b', 'c', 'm', 'j', 'd', 'e', 'p', 'n', 'k', 'f', 'g', 'r', 'q', 'o', 'l', 'h', 'a'}
        << vector<char> {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r'};

    /******************************************************************************************************************/
    QTest::newRow("Increasingly wide")
        << nary_tree<char>(
               n('a')(
                   n('b')(
                       n('d')(
                           n('j'),
                           n('k'),
                           n('l'),
                           n('m')),
                       n('e')(
                           n('n'),
                           n('o'),
                           n('p'),
                           n('q')),
                       n('f')(
                           n('r'),
                           n('s'),
                           n('t'),
                           n('u'))),
                   n('c')(
                       n('g')(
                           n('v'),
                           n('x'),
                           n('y'),
                           n('z')),
                       n('h')(
                           n('0'),
                           n('1'),
                           n('2'),
                           n('3')),
                       n('i')(
                           n('4'),
                           n('5'),
                           n('6'),
                           n('7')))))
        << 33
        << 4
        << vector<char> {'a', 'b', 'd', 'j', 'k', 'l', 'm', 'e', 'n', 'o', 'p', 'q', 'f', 'r', 's', 't', 'u',
                         'c', 'g', 'v', 'x', 'y', 'z', 'h', '0', '1', '2', '3', 'i', '4', '5', '6', '7'}
        << vector<char> {'j', 'k', 'l', 'm', 'd', 'n', 'o', 'p', 'q', 'e', 'r', 's', 't', 'u', 'f', 'b', 'v',
                         'x', 'y', 'z', 'g', '0', '1', '2', '3', 'h', '4', '5', '6', '7', 'i', 'c', 'a'}
        << vector<char> {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q',
                         'r', 's', 't', 'u', 'v', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7'};
}

QTEST_MAIN(NaryTreeIterationTest);
#include "NaryTreeIterationTest.moc"
