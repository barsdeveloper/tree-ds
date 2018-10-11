#include <QtTest/QtTest>
#include <algorithm>
#include <iterator>
#include <list>

#include <TreeDS/nary_tree>

using namespace std;
using namespace ds;

Q_DECLARE_METATYPE(list<char>);
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
    QFETCH(list<char>, expectedPreOrder);
    QFETCH(list<char>, expectedPostOrder);
    QFETCH(list<char>, expectedBreadthFirst);
    int actualSize = static_cast<int>(t.size());

    // Preliminary test
    QCOMPARE(actualSize, expectedSize);

    /*   ---   Forward order test   ---   */
    list<char> actualPreOrder;
    list<char> actualPostOrder;
    list<char> actualBreadthFirst;

    copy(t.begin<pre_order>(), t.end<pre_order>(), back_inserter(actualPreOrder));
    copy(t.begin<post_order>(), t.end<post_order>(), back_inserter(actualPostOrder));
    copy(
        t.begin<breadth_first<nary_node<char>>>(),
        t.end<breadth_first<nary_node<char>>>(),
        back_inserter(actualBreadthFirst));

    QCOMPARE(actualPreOrder, expectedPreOrder);
    QCOMPARE(actualPostOrder, expectedPostOrder);
    QCOMPARE(actualBreadthFirst, expectedBreadthFirst);

    /*   ---   Backward order test   ---   */
    list<char> actualNaryReversePreOrder;
    list<char> actualNaryReversePostOrder;
    list<char> actualNaryReverseBreadthFirst;

    list<char> expectedReversePreOrder;
    list<char> expectedReverseInOrder;
    list<char> expectedReversePostOrder;
    list<char> expectedReverseBreadthFirst;

    copy(t.rbegin<pre_order>(), t.rend<pre_order>(), back_inserter(actualNaryReversePreOrder));
    copy(t.rbegin<post_order>(), t.rend<post_order>(), back_inserter(actualNaryReversePostOrder));
    copy(
        t.rbegin<breadth_first<nary_node<char>>>(),
        t.rend<breadth_first<nary_node<char>>>(),
        back_inserter(actualNaryReverseBreadthFirst));

    copy(expectedPreOrder.rbegin(), expectedPreOrder.rend(), back_inserter(expectedReversePreOrder));
    copy(expectedPostOrder.rbegin(), expectedPostOrder.rend(), back_inserter(expectedReversePostOrder));
    copy(expectedBreadthFirst.rbegin(), expectedBreadthFirst.rend(), back_inserter(expectedReverseBreadthFirst));

    QCOMPARE(actualNaryReversePreOrder, expectedReversePreOrder);
    QCOMPARE(actualNaryReversePostOrder, expectedReversePostOrder);
    QCOMPARE(actualNaryReverseBreadthFirst, expectedReverseBreadthFirst);
}

void NaryTreeIterationTest::iteration_data() {
    QTest::addColumn<nary_tree<char>>("t");
    QTest::addColumn<int>("expectedSize");
    QTest::addColumn<list<char>>("expectedPreOrder");
    QTest::addColumn<list<char>>("expectedPostOrder");
    QTest::addColumn<list<char>>("expectedBreadthFirst");

    /******************************************************************************************************************/
    QTest::newRow("Single character")
        << nary_tree<char>(n('#'))
        << 1
        << list<char>{'#'}
        << list<char>{'#'}
        << list<char>{'#'};

    /******************************************************************************************************************/
    QTest::newRow("Root with a left child")

        << nary_tree<char>(
               n('1')(
                   n('2')))

        << 2
        << list<char>{'1', '2'}
        << list<char>{'2', '1'}
        << list<char>{'1', '2'};

    /******************************************************************************************************************/
    QTest::newRow("Root with a right child")

        << nary_tree<char>(
               n('1')(
                   n('2')))

        << 2
        << list<char>{'1', '2'}
        << list<char>{'2', '1'}
        << list<char>{'1', '2'};

    /******************************************************************************************************************/
    QTest::newRow("Small tree")

        << nary_tree<char>(
               n('a')(
                   n('b')(
                       n('d')(
                           n('h')),
                       n('e')),
                   n('c')(
                       n('f')(
                           n('j'),
                           n('k')),
                       n('g'))))

        << 10
        << list<char>{'a', 'b', 'd', 'h', 'e', 'c', 'f', 'j', 'k', 'g'}
        << list<char>{'h', 'd', 'e', 'b', 'j', 'k', 'f', 'g', 'c', 'a'}
        << list<char>{'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'j', 'k'};

    /******************************************************************************************************************/
    QTest::newRow("Big tree")

        << nary_tree<char>(
               n('a')(
                   n('b')(
                       n('c')(
                           n('d')(
                               n('e')(
                                   n('f'),
                                   n('g')),
                               n('h')(
                                   n('o'))),
                           n('i')(
                               n('n')(
                                   n('p')))),
                       n('j')(
                           n('m')(
                               n('q')(
                                   n('t'))))),
                   n('k')(
                       n('l')(
                           n('r')(
                               n('s')(
                                   n('u')))))))

        << 21
        << list<char>{'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'o', 'i', 'n', 'p', 'j', 'm', 'q', 't', 'k', 'l', 'r', 's', 'u'}
        << list<char>{'f', 'g', 'e', 'o', 'h', 'd', 'p', 'n', 'i', 'c', 't', 'q', 'm', 'j', 'b', 'u', 's', 'r', 'l', 'k', 'a'}
        << list<char>{'a', 'b', 'k', 'c', 'j', 'l', 'd', 'i', 'm', 'r', 'e', 'h', 'n', 'q', 's', 'f', 'g', 'o', 'p', 't', 'u'};

    /******************************************************************************************************************/
    QTest::newRow("All left child")

        << nary_tree<char>(
               n('1')(
                   n('2')(
                       n('3')(
                           n('4')(
                               n('5')(
                                   n('6')(
                                       n('7')(
                                           n('8')(
                                               n('9')(
                                                   n('A')(
                                                       n('B')(
                                                           n('C')(
                                                               n('D')(
                                                                   n('E')(
                                                                       n('F'))))))))))))))))

        << 15
        << list<char>{'1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'}
        << list<char>{'F', 'E', 'D', 'C', 'B', 'A', '9', '8', '7', '6', '5', '4', '3', '2', '1'}
        << list<char>{'1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

    /******************************************************************************************************************/
    QTest::newRow("All right child")

        << nary_tree<char>(
               n('1')(
                   n('2')(
                       n('3')(
                           n('4')(
                               n('5')(
                                   n('6')(
                                       n('7')(
                                           n('8')(
                                               n('9')(
                                                   n('A')(
                                                       n('B')(
                                                           n('C')(
                                                               n('D')(
                                                                   n('E')(
                                                                       n('F'))))))))))))))))

        << 15
        << list<char>{'1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'}
        << list<char>{'F', 'E', 'D', 'C', 'B', 'A', '9', '8', '7', '6', '5', '4', '3', '2', '1'}
        << list<char>{'1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

    /******************************************************************************************************************/
    QTest::newRow("ZigZag")

        << nary_tree<char>(
               n('1')(
                   n('2')(
                       n('3')(
                           n('4')(
                               n('5')(
                                   n('6')(
                                       n('7')(
                                           n('8')(
                                               n('9')(
                                                   n('A')(
                                                       n('B')(
                                                           n('C')(
                                                               n('D')(
                                                                   n('E')(
                                                                       n('F'))))))))))))))))

        << 15
        << list<char>{'1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'}
        << list<char>{'F', 'E', 'D', 'C', 'B', 'A', '9', '8', '7', '6', '5', '4', '3', '2', '1'}
        << list<char>{'1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
}

QTEST_MAIN(NaryTreeIterationTest);
#include "NaryTreeIterationTest.moc"
