#include <QtTest/QtTest>
#include <algorithm>
#include <iterator>
#include <vector>

#include <TreeDS/tree>
#include <TreeDS/view>

using namespace std;
using namespace md;

Q_DECLARE_METATYPE(vector<int>);
Q_DECLARE_METATYPE(binary_tree_view<int>);
Q_DECLARE_METATYPE(nary_tree_view<int>);

class TreeViewIterationTest : public QObject {

    Q_OBJECT

    binary_tree<int> bin_tree
        = n(1)(
            n(2)(
                n(4)(
                    n(),
                    n(6)(
                        n(8)(
                            n(12),
                            n(13)),
                        n(9)(
                            n(14),
                            n(15)(
                                n(20)(
                                    n(),
                                    n(21)(
                                        n(22)(
                                            n(23))))))))),
            n(3)(
                n(),
                n(5)(
                    n(7)(
                        n(10)(
                            n(16),
                            n(17)),
                        n(11)(
                            n(18),
                            n(19))))));

    nary_tree<int> nar_tree = bin_tree;

    private slots:
    void iteration();
    void iteration_data();
};

void TreeViewIterationTest::iteration() {
    QFETCH(binary_tree_view<int>, binary);
    QFETCH(nary_tree_view<int>, nary);
    QFETCH(int, expectedSize);
    QFETCH(int, expectedArity);
    QFETCH(vector<int>, expectedPreOrder);
    QFETCH(vector<int>, expectedInOrder);
    QFETCH(vector<int>, expectedPostOrder);
    QFETCH(vector<int>, expectedBreadthFirst);
    int actualSize  = static_cast<int>(binary.size());
    int actualArity = static_cast<int>(binary.arity());

    // Preliminary test
    QCOMPARE(nary, binary);
    QCOMPARE(actualSize, expectedSize);
    QCOMPARE(actualArity, expectedArity);

    /*   ---   Forward order test   ---   */
    // Binary
    vector<int> actualBinaryPreOrder(binary.begin(policy::pre_order()), binary.end(policy::pre_order()));
    vector<int> actualBinaryInOrder(binary.begin(policy::in_order()), binary.end(policy::in_order()));
    vector<int> actualBinaryPostOrder(binary.begin(policy::post_order()), binary.end(policy::post_order()));
    vector<int> actualBinaryBreadthFirst(binary.begin(policy::breadth_first()), binary.end(policy::breadth_first()));

    // Compare results binary
    QCOMPARE(actualBinaryPreOrder, expectedPreOrder);
    QCOMPARE(actualBinaryInOrder, expectedInOrder);
    QCOMPARE(actualBinaryPostOrder, expectedPostOrder);
    QCOMPARE(actualBinaryBreadthFirst, expectedBreadthFirst);

    // Nary
    vector<int> actualNaryPreOrder(nary.begin(policy::pre_order()), nary.end(policy::pre_order()));
    vector<int> actualNaryPostOrder(nary.begin(policy::post_order()), nary.end(policy::post_order()));
    vector<int> actualNaryBreadthFirst(nary.begin(policy::breadth_first()), nary.end(policy::breadth_first()));

    // Compare results nary
    QCOMPARE(actualNaryPreOrder, expectedPreOrder);
    QCOMPARE(actualNaryPostOrder, expectedPostOrder);
    QCOMPARE(actualNaryBreadthFirst, expectedBreadthFirst);

    /*   ---   Backward order test   ---   */
    vector<char> expectedReversePreOrder(expectedPreOrder.rbegin(), expectedPreOrder.rend());
    vector<char> expectedReverseInOrder(expectedInOrder.rbegin(), expectedInOrder.rend());
    vector<char> expectedReversePostOrder(expectedPostOrder.rbegin(), expectedPostOrder.rend());
    vector<char> expectedReverseBreadthFirst(expectedBreadthFirst.rbegin(), expectedBreadthFirst.rend());

    // Binary
    vector<char> actualBinaryReversePreOrder(binary.rbegin(policy::pre_order()), binary.rend(policy::pre_order()));
    vector<char> actualBinaryReverseInOrder(binary.rbegin(policy::in_order()), binary.rend(policy::in_order()));
    vector<char> actualBinaryReversePostOrder(binary.rbegin(policy::post_order()), binary.rend(policy::post_order()));
    vector<char> actualBinaryReverseBreadthFirst(binary.rbegin(policy::breadth_first()), binary.rend(policy::breadth_first()));

    // Compare results binary
    QCOMPARE(actualBinaryReversePreOrder, expectedReversePreOrder);
    QCOMPARE(actualBinaryReverseInOrder, expectedReverseInOrder);
    QCOMPARE(actualBinaryReversePostOrder, expectedReversePostOrder);
    QCOMPARE(actualBinaryReverseBreadthFirst, expectedReverseBreadthFirst);

    // Nary
    vector<char> actualNaryReversePreOrder(nary.rbegin(policy::pre_order()), nary.rend(policy::pre_order()));
    vector<char> actualNaryReversePostOrder(nary.rbegin(policy::post_order()), nary.rend(policy::post_order()));
    vector<char> actualNaryReverseBreadthFirst(nary.rbegin(policy::breadth_first()), nary.rend(policy::breadth_first()));

    // Compare results nary
    QCOMPARE(actualNaryReversePreOrder, expectedReversePreOrder);
    QCOMPARE(actualNaryReversePostOrder, expectedReversePostOrder);
    QCOMPARE(actualNaryReverseBreadthFirst, expectedReverseBreadthFirst);
}

void TreeViewIterationTest::iteration_data() {
    QTest::addColumn<binary_tree_view<int>>("binary");
    QTest::addColumn<nary_tree_view<int>>("nary");
    QTest::addColumn<int>("expectedSize");
    QTest::addColumn<int>("expectedArity");
    QTest::addColumn<vector<int>>("expectedPreOrder");
    QTest::addColumn<vector<int>>("expectedInOrder");
    QTest::addColumn<vector<int>>("expectedPostOrder");
    QTest::addColumn<vector<int>>("expectedBreadthFirst");

    /******************************************************************************************************************/
    QTest::newRow("Straight")
        << binary_tree_view<int>(this->bin_tree)
        << nary_tree_view<int>(this->nar_tree)
        << 23
        << 2
        << vector {1, 2, 4, 6, 8, 12, 13, 9, 14, 15, 20, 21, 22, 23, 3, 5, 7, 10, 16, 17, 11, 18, 19}
        << vector {4, 12, 8, 13, 6, 14, 9, 20, 23, 22, 21, 15, 2, 1, 3, 16, 10, 17, 7, 18, 11, 19, 5}
        << vector {12, 13, 8, 14, 23, 22, 21, 20, 15, 9, 6, 4, 2, 16, 17, 10, 18, 19, 11, 7, 5, 3, 1}
        << vector {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23};

    /******************************************************************************************************************/
    QTest::newRow("From begin")
        << binary_tree_view<int>(this->bin_tree, this->bin_tree.begin())
        << nary_tree_view<int>(this->nar_tree, this->nar_tree.begin())
        << 23
        << 2
        << vector {1, 2, 4, 6, 8, 12, 13, 9, 14, 15, 20, 21, 22, 23, 3, 5, 7, 10, 16, 17, 11, 18, 19}
        << vector {4, 12, 8, 13, 6, 14, 9, 20, 23, 22, 21, 15, 2, 1, 3, 16, 10, 17, 7, 18, 11, 19, 5}
        << vector {12, 13, 8, 14, 23, 22, 21, 20, 15, 9, 6, 4, 2, 16, 17, 10, 18, 19, 11, 7, 5, 3, 1}
        << vector {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23};

    /******************************************************************************************************************/
    QTest::newRow("From 1")
        << binary_tree_view<int>(this->bin_tree, std::find(this->bin_tree.begin(), this->bin_tree.end(), 1))
        << nary_tree_view<int>(this->nar_tree, std::find(this->nar_tree.begin(), this->nar_tree.end(), 1))
        << 23
        << 2
        << vector {1, 2, 4, 6, 8, 12, 13, 9, 14, 15, 20, 21, 22, 23, 3, 5, 7, 10, 16, 17, 11, 18, 19}
        << vector {4, 12, 8, 13, 6, 14, 9, 20, 23, 22, 21, 15, 2, 1, 3, 16, 10, 17, 7, 18, 11, 19, 5}
        << vector {12, 13, 8, 14, 23, 22, 21, 20, 15, 9, 6, 4, 2, 16, 17, 10, 18, 19, 11, 7, 5, 3, 1}
        << vector {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23};

    /******************************************************************************************************************/
    QTest::newRow("From 2")
        << binary_tree_view<int>(this->bin_tree, std::find(this->bin_tree.begin(), this->bin_tree.end(), 2))
        << nary_tree_view<int>(this->nar_tree, std::find(this->nar_tree.begin(), this->nar_tree.end(), 2))
        << 13
        << 2
        << vector {2, 4, 6, 8, 12, 13, 9, 14, 15, 20, 21, 22, 23}
        << vector {4, 12, 8, 13, 6, 14, 9, 20, 23, 22, 21, 15, 2}
        << vector {12, 13, 8, 14, 23, 22, 21, 20, 15, 9, 6, 4, 2}
        << vector {2, 4, 6, 8, 9, 12, 13, 14, 15, 20, 21, 22, 23};

    /******************************************************************************************************************/
    QTest::newRow("From 4")
        << binary_tree_view<int>(this->bin_tree, std::find(this->bin_tree.begin(), this->bin_tree.end(), 4))
        << nary_tree_view<int>(this->nar_tree, std::find(this->nar_tree.begin(), this->nar_tree.end(), 4))
        << 12
        << 2
        << vector {4, 6, 8, 12, 13, 9, 14, 15, 20, 21, 22, 23}
        << vector {4, 12, 8, 13, 6, 14, 9, 20, 23, 22, 21, 15}
        << vector {12, 13, 8, 14, 23, 22, 21, 20, 15, 9, 6, 4}
        << vector {4, 6, 8, 9, 12, 13, 14, 15, 20, 21, 22, 23};

    /******************************************************************************************************************/
    QTest::newRow("From 6")
        << binary_tree_view<int>(this->bin_tree, std::find(this->bin_tree.begin(), this->bin_tree.end(), 6))
        << nary_tree_view<int>(this->nar_tree, std::find(this->nar_tree.begin(), this->nar_tree.end(), 6))
        << 11
        << 2
        << vector {6, 8, 12, 13, 9, 14, 15, 20, 21, 22, 23}
        << vector {12, 8, 13, 6, 14, 9, 20, 23, 22, 21, 15}
        << vector {12, 13, 8, 14, 23, 22, 21, 20, 15, 9, 6}
        << vector {6, 8, 9, 12, 13, 14, 15, 20, 21, 22, 23};

    /******************************************************************************************************************/
    QTest::newRow("From 8")
        << binary_tree_view<int>(this->bin_tree, std::find(this->bin_tree.begin(), this->bin_tree.end(), 8))
        << nary_tree_view<int>(this->nar_tree, std::find(this->nar_tree.begin(), this->nar_tree.end(), 8))
        << 3
        << 2
        << vector {8, 12, 13}
        << vector {12, 8, 13}
        << vector {12, 13, 8}
        << vector {8, 12, 13};

    /******************************************************************************************************************/
    QTest::newRow("From 14")
        << binary_tree_view<int>(this->bin_tree, std::find(this->bin_tree.begin(), this->bin_tree.end(), 14))
        << nary_tree_view<int>(this->nar_tree, std::find(this->nar_tree.begin(), this->nar_tree.end(), 14))
        << 1
        << 0
        << vector {14}
        << vector {14}
        << vector {14}
        << vector {14};

    /******************************************************************************************************************/
    QTest::newRow("From 15")
        << binary_tree_view<int>(this->bin_tree, std::find(this->bin_tree.begin(), this->bin_tree.end(), 15))
        << nary_tree_view<int>(this->nar_tree, std::find(this->nar_tree.begin(), this->nar_tree.end(), 15))
        << 5
        << 1
        << vector {15, 20, 21, 22, 23}
        << vector {20, 23, 22, 21, 15}
        << vector {23, 22, 21, 20, 15}
        << vector {15, 20, 21, 22, 23};

    /******************************************************************************************************************/
    QTest::newRow("From 13")
        << binary_tree_view<int>(this->bin_tree, std::find(this->bin_tree.begin(), this->bin_tree.end(), 3))
        << nary_tree_view<int>(this->nar_tree, std::find(this->nar_tree.begin(), this->nar_tree.end(), 3))
        << 9
        << 2
        << vector {3, 5, 7, 10, 16, 17, 11, 18, 19}
        << vector {3, 16, 10, 17, 7, 18, 11, 19, 5}
        << vector {16, 17, 10, 18, 19, 11, 7, 5, 3}
        << vector {3, 5, 7, 10, 11, 16, 17, 18, 19};

    /******************************************************************************************************************/
    QTest::newRow("From end")
        << binary_tree_view<int>(this->bin_tree, this->bin_tree.end())
        << nary_tree_view<int>(this->nar_tree, this->nar_tree.end())
        << 0
        << 0
        << vector<int>()
        << vector<int>()
        << vector<int>()
        << vector<int>();
}

QTEST_MAIN(TreeViewIterationTest);
#include "TreeViewIterationTest.moc"
