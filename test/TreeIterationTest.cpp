#include <QtTest/QtTest>
#include <algorithm>
#include <iterator>
#include <list>

#include <TreeDS/tree>

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
    QFETCH(binary_tree<char>, binary);
    QFETCH(int, expectedSize);
    QFETCH(list<char>, expectedPreOrder);
    QFETCH(list<char>, expectedInOrder);
    QFETCH(list<char>, expectedPostOrder);
    QFETCH(list<char>, expectedBreadthFirst);
    nary_tree<char> nary(binary);
    int actualSize = static_cast<int>(binary.size());

    // Preliminary test
    QCOMPARE(nary, binary);
    QCOMPARE(actualSize, expectedSize);

    /*   ---   Forward order test   ---   */
    // Binary
    list<char> actualBinaryPreOrder;
    list<char> actualBinaryInOrder;
    list<char> actualBinaryPostOrder;
    list<char> actualBinaryBreadthFirst;
    // Nary
    list<char> actualNaryPreOrder;
    list<char> actualNaryInOrder;
    list<char> actualNaryPostOrder;
    list<char> actualNaryBreadthFirst;

    // Traverse binary
    copy(binary.begin<pre_order>(), binary.end<pre_order>(), back_inserter(actualBinaryPreOrder));
    copy(binary.begin<in_order>(), binary.end<in_order>(), back_inserter(actualBinaryInOrder));
    copy(binary.begin<post_order>(), binary.end<post_order>(), back_inserter(actualBinaryPostOrder));
    copy(
        binary.begin<breadth_first<binary_node<char>>>(),
        binary.end<breadth_first<binary_node<char>>>(),
        back_inserter(actualBinaryBreadthFirst));
    // Traverse nary
    copy(nary.begin<pre_order>(), nary.end<pre_order>(), back_inserter(actualNaryPreOrder));
    copy(nary.begin<post_order>(), nary.end<post_order>(), back_inserter(actualNaryPostOrder));
    copy(
        nary.begin<breadth_first<nary_node<char>>>(),
        nary.end<breadth_first<nary_node<char>>>(),
        back_inserter(actualNaryBreadthFirst));

    // Compare results binary
    QCOMPARE(actualBinaryPreOrder, expectedPreOrder);
    QCOMPARE(actualBinaryInOrder, expectedInOrder);
    QCOMPARE(actualBinaryPostOrder, expectedPostOrder);
    QCOMPARE(actualBinaryBreadthFirst, expectedBreadthFirst);
    // Compare results nary
    QCOMPARE(actualNaryPreOrder, expectedPreOrder);
    QCOMPARE(actualNaryPostOrder, expectedPostOrder);
    QCOMPARE(actualNaryBreadthFirst, expectedBreadthFirst);

    /*   ---   Backward order test   ---   */
    // Binary
    list<char> actualBinaryReversePreOrder;
    list<char> actualBinaryReverseInOrder;
    list<char> actualBinaryReversePostOrder;
    list<char> actualBinaryReverseBreadthFirst;
    // Nary
    list<char> actualNaryReversePreOrder;
    list<char> actualNaryReversePostOrder;
    list<char> actualNaryReverseBreadthFirst;

    list<char> expectedReversePreOrder;
    list<char> expectedReverseInOrder;
    list<char> expectedReversePostOrder;
    list<char> expectedReverseBreadthFirst;

    // Reverse traverse binary
    copy(binary.rbegin<pre_order>(), binary.rend<pre_order>(), back_inserter(actualBinaryReversePreOrder));
    copy(binary.rbegin<in_order>(), binary.rend<in_order>(), back_inserter(actualBinaryReverseInOrder));
    copy(binary.rbegin<post_order>(), binary.rend<post_order>(), back_inserter(actualBinaryReversePostOrder));
    copy(
        binary.rbegin<breadth_first<binary_node<char>>>(),
        binary.rend<breadth_first<binary_node<char>>>(),
        back_inserter(actualBinaryReverseBreadthFirst));

    // Reverse traverse nary
    copy(nary.rbegin<pre_order>(), nary.rend<pre_order>(), back_inserter(actualNaryReversePreOrder));
    copy(nary.rbegin<post_order>(), nary.rend<post_order>(), back_inserter(actualNaryReversePostOrder));
    copy(
        nary.rbegin<breadth_first<nary_node<char>>>(),
        nary.rend<breadth_first<nary_node<char>>>(),
        back_inserter(actualNaryReverseBreadthFirst));

    // Reverse expected
    copy(expectedPreOrder.rbegin(), expectedPreOrder.rend(), back_inserter(expectedReversePreOrder));
    copy(expectedInOrder.rbegin(), expectedInOrder.rend(), back_inserter(expectedReverseInOrder));
    copy(expectedPostOrder.rbegin(), expectedPostOrder.rend(), back_inserter(expectedReversePostOrder));
    copy(expectedBreadthFirst.rbegin(), expectedBreadthFirst.rend(), back_inserter(expectedReverseBreadthFirst));

    // Compare results binary
    QCOMPARE(actualBinaryReversePreOrder, expectedReversePreOrder);
    QCOMPARE(actualBinaryReverseInOrder, expectedReverseInOrder);
    QCOMPARE(actualBinaryReversePostOrder, expectedReversePostOrder);
    QCOMPARE(actualBinaryReverseBreadthFirst, expectedReverseBreadthFirst);
    // Compare results nary
    QCOMPARE(actualNaryReversePreOrder, expectedReversePreOrder);
    QCOMPARE(actualNaryReversePostOrder, expectedReversePostOrder);
    QCOMPARE(actualNaryReverseBreadthFirst, expectedReverseBreadthFirst);
}

void TreeIterationTest::iteration_data() {
    QTest::addColumn<binary_tree<char>>("binary");
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
