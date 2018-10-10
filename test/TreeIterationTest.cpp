#include <QtTest/QtTest>
#include <algorithm>
#include <iterator>
#include <list>

#include <TreeDS/binary_tree>

using namespace std;
using namespace ds;

Q_DECLARE_METATYPE(list<char>);
Q_DECLARE_METATYPE(binary_tree<char>);

class TreeIterationTest : public QObject {

    Q_OBJECT

    private slots:
    void iteration();
    void iteration_data();
};

void TreeIterationTest::iteration() {
    QFETCH(binary_tree<char>, t);
    QFETCH(int, expectedSize);
    QFETCH(list<char>, expectedPreOrder);
    QFETCH(list<char>, expectedInOrder);
    QFETCH(list<char>, expectedPostOrder);
    QFETCH(list<char>, expectedBreadthFirst);

    list<char> actualPreOrder;
    list<char> actualInOrder;
    list<char> actualPostOrder;
    list<char> actualBreadthFirst;
    int actualSize = static_cast<int>(t.size());

    copy(t.begin<pre_order>(), t.end<pre_order>(), back_inserter(actualPreOrder));
    copy(t.begin<in_order>(), t.end<in_order>(), back_inserter(actualInOrder));
    copy(t.begin<post_order>(), t.end<post_order>(), back_inserter(actualPostOrder));
    copy(
        t.begin<breadth_first<binary_node<char>>>(),
        t.end<breadth_first<binary_node<char>>>(),
        back_inserter(actualBreadthFirst));

    QCOMPARE(actualSize, expectedSize);
    QCOMPARE(actualPreOrder, expectedPreOrder);
    QCOMPARE(actualInOrder, expectedInOrder);
    QCOMPARE(actualPostOrder, expectedPostOrder);
    QCOMPARE(actualBreadthFirst, expectedBreadthFirst);

    /*   ---   Reverse order test   ---   */
    list<char> actualReversePreOrder;
    list<char> actualReverseInOrder;
    list<char> actualReversePostOrder;
    list<char> actualReverseBreadthFirst;

    list<char> expectedReversePreOrder;
    list<char> expectedReverseInOrder;
    list<char> expectedReversePostOrder;
    list<char> expectedReverseBreadthFirst;

    copy(t.rbegin<pre_order>(), t.rend<pre_order>(), back_inserter(actualReversePreOrder));
    copy(t.rbegin<in_order>(), t.rend<in_order>(), back_inserter(actualReverseInOrder));
    copy(t.rbegin<post_order>(), t.rend<post_order>(), back_inserter(actualReversePostOrder));
    copy(
        t.rbegin<breadth_first<binary_node<char>>>(),
        t.rend<breadth_first<binary_node<char>>>(),
        back_inserter(actualReverseBreadthFirst));

    copy(expectedPreOrder.rbegin(), expectedPreOrder.rend(), back_inserter(expectedReversePreOrder));
    copy(expectedInOrder.rbegin(), expectedInOrder.rend(), back_inserter(expectedReverseInOrder));
    copy(expectedPostOrder.rbegin(), expectedPostOrder.rend(), back_inserter(expectedReversePostOrder));
    copy(expectedBreadthFirst.rbegin(), expectedBreadthFirst.rend(), back_inserter(expectedReverseBreadthFirst));

    QCOMPARE(actualReversePreOrder, expectedReversePreOrder);
    QCOMPARE(actualReverseInOrder, expectedReverseInOrder);
    QCOMPARE(actualReversePostOrder, expectedReversePostOrder);
    QCOMPARE(actualReverseBreadthFirst, expectedReverseBreadthFirst);
}

void TreeIterationTest::iteration_data() {
    QTest::addColumn<binary_tree<char>>("t");
    QTest::addColumn<int>("expectedSize");
    QTest::addColumn<list<char>>("expectedPreOrder");
    QTest::addColumn<list<char>>("expectedInOrder");
    QTest::addColumn<list<char>>("expectedPostOrder");
    QTest::addColumn<list<char>>("expectedBreadthFirst");

    /******************************************************************************************************************/
    QTest::newRow("Single character")
        << binary_tree<char>(n('#'))
        << 1
        << list<char>{'#'}
        << list<char>{'#'}
        << list<char>{'#'}
        << list<char>{'#'};

    /******************************************************************************************************************/
    QTest::newRow("Root with a left child")

        << binary_tree<char>(
               n('1')(
                   n('2')))

        << 2
        << list<char>{'1', '2'}
        << list<char>{'2', '1'}
        << list<char>{'2', '1'}
        << list<char>{'1', '2'};

    /******************************************************************************************************************/
    QTest::newRow("Root with a right child")

        << binary_tree<char>(
               n('1')(
                   n(),
                   n('2')))

        << 2
        << list<char>{'1', '2'}
        << list<char>{'1', '2'}
        << list<char>{'2', '1'}
        << list<char>{'1', '2'};

    /******************************************************************************************************************/
    QTest::newRow("Small tree")

        << binary_tree<char>(
               n('a')(
                   n('b')(
                       n('d')(
                           n('h'),
                           n() // This can be omitted but I want to test it
                           ),
                       n('e')),
                   n('c')(
                       n('f')(
                           n('j'),
                           n('k')),
                       n('g'))))

        << 10
        << list<char>{'a', 'b', 'd', 'h', 'e', 'c', 'f', 'j', 'k', 'g'}
        << list<char>{'h', 'd', 'b', 'e', 'a', 'j', 'f', 'k', 'c', 'g'}
        << list<char>{'h', 'd', 'e', 'b', 'j', 'k', 'f', 'g', 'c', 'a'}
        << list<char>{'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'j', 'k'};

    /******************************************************************************************************************/
    QTest::newRow("Big tree")

        << binary_tree<char>(
               n('a')(
                   n('b')(
                       n('c')(
                           n('d')(
                               n('e')(
                                   n('f'),
                                   n('g')),
                               n('h')(
                                   n(),
                                   n('o'))),
                           n('i')(
                               n(),
                               n('n')(
                                   n(),
                                   n('p')))),
                       n('j')(
                           n(),
                           n('m')(
                               n(),
                               n('q')(
                                   n(),
                                   n('t'))))),
                   n('k')(
                       n(),
                       n('l')(
                           n(),
                           n('r')(
                               n(),
                               n('s')(
                                   n(),
                                   n('u')))))))

        << 21
        << list<char>{'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'o', 'i', 'n', 'p', 'j', 'm', 'q', 't', 'k', 'l', 'r', 's', 'u'}
        << list<char>{'f', 'e', 'g', 'd', 'h', 'o', 'c', 'i', 'n', 'p', 'b', 'j', 'm', 'q', 't', 'a', 'k', 'l', 'r', 's', 'u'}
        << list<char>{'f', 'g', 'e', 'o', 'h', 'd', 'p', 'n', 'i', 'c', 't', 'q', 'm', 'j', 'b', 'u', 's', 'r', 'l', 'k', 'a'}
        << list<char>{'a', 'b', 'k', 'c', 'j', 'l', 'd', 'i', 'm', 'r', 'e', 'h', 'n', 'q', 's', 'f', 'g', 'o', 'p', 't', 'u'};

    /******************************************************************************************************************/
    QTest::newRow("All left child")

        << binary_tree<char>(
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
        << list<char>{'F', 'E', 'D', 'C', 'B', 'A', '9', '8', '7', '6', '5', '4', '3', '2', '1'}
        << list<char>{'1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

    /******************************************************************************************************************/
    QTest::newRow("All right child")

        << binary_tree<char>(
               n('1')(
                   n(),
                   n('2')(
                       n(),
                       n('3')(
                           n(),
                           n('4')(
                               n(),
                               n('5')(
                                   n(),
                                   n('6')(
                                       n(),
                                       n('7')(
                                           n(),
                                           n('8')(
                                               n(),
                                               n('9')(
                                                   n(),
                                                   n('A')(
                                                       n(),
                                                       n('B')(
                                                           n(),
                                                           n('C')(
                                                               n(),
                                                               n('D')(
                                                                   n(),
                                                                   n('E')(
                                                                       n(),
                                                                       n('F'))))))))))))))))

        << 15
        << list<char>{'1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'}
        << list<char>{'1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'}
        << list<char>{'F', 'E', 'D', 'C', 'B', 'A', '9', '8', '7', '6', '5', '4', '3', '2', '1'}
        << list<char>{'1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

    /******************************************************************************************************************/
    QTest::newRow("ZigZag")

        << binary_tree<char>(
               n('1')(
                   n('2')(
                       n(),
                       n('3')(
                           n('4')(
                               n(),
                               n('5')(
                                   n('6')(
                                       n(),
                                       n('7')(
                                           n('8')(
                                               n(),
                                               n('9')(
                                                   n('A')(
                                                       n(),
                                                       n('B')(
                                                           n('C')(
                                                               n(),
                                                               n('D')(
                                                                   n('E')(
                                                                       n(),
                                                                       n('F'))))))))))))))))

        << 15
        << list<char>{'1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'}
        << list<char>{'2', '4', '6', '8', 'A', 'C', 'E', 'F', 'D', 'B', '9', '7', '5', '3', '1'}
        << list<char>{'F', 'E', 'D', 'C', 'B', 'A', '9', '8', '7', '6', '5', '4', '3', '2', '1'}
        << list<char>{'1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
}

QTEST_MAIN(TreeIterationTest);
#include "TreeIterationTest.moc"
