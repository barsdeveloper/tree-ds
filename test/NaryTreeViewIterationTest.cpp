#include <QtTest/QtTest>
#include <vector>

#include <TreeDS/tree>
#include <TreeDS/view>

using namespace md;
using namespace std;

Q_DECLARE_METATYPE(nary_tree_view<char>);

class NaryTreeViewIterationTest : public QObject {

    Q_OBJECT

    nary_tree<char> tree
        = n('a')(
            n('b'),
            n('c')(
                n('f'),
                n('g')(
                    n('k'),
                    n('l')),
                n('h')(
                    n('m')(
                        n('s')(
                            n('u')(
                                n('z')(
                                    n('1')(
                                        n('3')(
                                            n('5'))))))),
                    n('n'),
                    n('o'),
                    n('p'),
                    n('q')(
                        n('t')(
                            n('v')(
                                n('0')(
                                    n('2'))))))),
            n('d')(
                n('i')),
            n('e')(
                n('j')(
                    n('r'))));

    private slots:
    void iteration();
    void iteration_data();
};

void NaryTreeViewIterationTest::iteration_data() {
    QTest::addColumn<nary_tree_view<char>>("view");
    QTest::addColumn<int>("expectedSize");
    QTest::addColumn<int>("expectedArity");
    QTest::addColumn<vector<char>>("expectedPreOrder");
    QTest::addColumn<vector<char>>("expectedPostOrder");
    QTest::addColumn<vector<char>>("expectedBreadthFirst");
    QTest::addColumn<vector<char>>("expectedLeaves");

    /******************************************************************************************************************/
    QTest::newRow("Straight")
        << nary_tree_view<char>(this->tree)
        << 28
        << 5
        << vector {'a', 'b', 'c', 'f', 'g', 'k', 'l', 'h', 'm', 's', 'u', 'z', '1', '3', '5', 'n', 'o', 'p', 'q', 't', 'v', '0', '2', 'd', 'i', 'e', 'j', 'r'}
        << vector {'b', 'f', 'k', 'l', 'g', '5', '3', '1', 'z', 'u', 's', 'm', 'n', 'o', 'p', '2', '0', 'v', 't', 'q', 'h', 'c', 'i', 'd', 'r', 'j', 'e', 'a'}
        << vector {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'z', '0', '1', '2', '3', '5'}
        << vector {'b', 'f', 'k', 'l', '5', 'n', 'o', 'p', '2', 'i', 'r'};

    /******************************************************************************************************************/
    QTest::newRow("From begin")
        << nary_tree_view<char>(this->tree, this->tree.begin())
        << 28
        << 5
        << vector {'a', 'b', 'c', 'f', 'g', 'k', 'l', 'h', 'm', 's', 'u', 'z', '1', '3', '5', 'n', 'o', 'p', 'q', 't', 'v', '0', '2', 'd', 'i', 'e', 'j', 'r'}
        << vector {'b', 'f', 'k', 'l', 'g', '5', '3', '1', 'z', 'u', 's', 'm', 'n', 'o', 'p', '2', '0', 'v', 't', 'q', 'h', 'c', 'i', 'd', 'r', 'j', 'e', 'a'}
        << vector {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'z', '0', '1', '2', '3', '5'}
        << vector {'b', 'f', 'k', 'l', '5', 'n', 'o', 'p', '2', 'i', 'r'};

    /******************************************************************************************************************/
    QTest::newRow("From c")
        << nary_tree_view<char>(this->tree, std::find(this->tree.begin(), this->tree.end(), 'c'))
        << 21
        << 5
        << vector {'c', 'f', 'g', 'k', 'l', 'h', 'm', 's', 'u', 'z', '1', '3', '5', 'n', 'o', 'p', 'q', 't', 'v', '0', '2'}
        << vector {'f', 'k', 'l', 'g', '5', '3', '1', 'z', 'u', 's', 'm', 'n', 'o', 'p', '2', '0', 'v', 't', 'q', 'h', 'c'}
        << vector {'c', 'f', 'g', 'h', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 's', 't', 'u', 'v', 'z', '0', '1', '2', '3', '5'}
        << vector {'f', 'k', 'l', '5', 'n', 'o', 'p', '2'};

    /******************************************************************************************************************/
    QTest::newRow("From g")
        << nary_tree_view<char>(this->tree, std::find(this->tree.begin(), this->tree.end(), 'g'))
        << 3
        << 2
        << vector {'g', 'k', 'l'}
        << vector {'k', 'l', 'g'}
        << vector {'g', 'k', 'l'}
        << vector {'k', 'l'};

    /******************************************************************************************************************/
    QTest::newRow("From h")
        << nary_tree_view<char>(this->tree, std::find(this->tree.begin(), this->tree.end(), 'h'))
        << 16
        << 5
        << vector {'h', 'm', 's', 'u', 'z', '1', '3', '5', 'n', 'o', 'p', 'q', 't', 'v', '0', '2'}
        << vector {'5', '3', '1', 'z', 'u', 's', 'm', 'n', 'o', 'p', '2', '0', 'v', 't', 'q', 'h'}
        << vector {'h', 'm', 'n', 'o', 'p', 'q', 's', 't', 'u', 'v', 'z', '0', '1', '2', '3', '5'}
        << vector {'5', 'n', 'o', 'p', '2'};

    /******************************************************************************************************************/
    QTest::newRow("From z")
        << nary_tree_view<char>(this->tree, std::find(this->tree.begin(), this->tree.end(), 'z'))
        << 4
        << 1
        << vector {'z', '1', '3', '5'}
        << vector {'5', '3', '1', 'z'}
        << vector {'z', '1', '3', '5'}
        << vector {'5'};

    /******************************************************************************************************************/
    QTest::newRow("From 0")
        << nary_tree_view<char>(this->tree, std::find(this->tree.begin(), this->tree.end(), '0'))
        << 2
        << 1
        << vector {'0', '2'}
        << vector {'2', '0'}
        << vector {'0', '2'}
        << vector {'2'};

    /******************************************************************************************************************/
    QTest::newRow("From n")
        << nary_tree_view<char>(this->tree, std::find(this->tree.begin(), this->tree.end(), 'n'))
        << 1
        << 0
        << vector {'n'}
        << vector {'n'}
        << vector {'n'}
        << vector {'n'};

    /******************************************************************************************************************/
    QTest::newRow("From end")
        << nary_tree_view<char>(this->tree, this->tree.end())
        << 0
        << 0
        << vector<char>()
        << vector<char>()
        << vector<char>()
        << vector<char>();
}

void NaryTreeViewIterationTest::iteration() {

    QFETCH(nary_tree_view<char>, view);
    QFETCH(int, expectedSize);
    QFETCH(int, expectedArity);
    QFETCH(vector<char>, expectedPreOrder);
    QFETCH(vector<char>, expectedPostOrder);
    QFETCH(vector<char>, expectedBreadthFirst);
    QFETCH(vector<char>, expectedLeaves);
    int actualSize  = static_cast<int>(view.size());
    int actualArity = static_cast<int>(view.arity());

    // Preliminary test
    QCOMPARE(actualSize, expectedSize);
    QCOMPARE(actualArity, expectedArity);

    auto it = view.begin(policy::pre_order());
    while (it.get_raw_node() != nullptr)
        ++it;
    QVERIFY(it == view.end(policy::pre_order()));

    /*   ---   Forward order test   ---   */
    vector<char> actualPreOrder(view.begin(policy::pre_order()), view.end(policy::pre_order()));
    vector<char> actualPostOrder(view.begin(policy::post_order()), view.end(policy::post_order()));
    vector<char> actualBreadthFirst(view.begin(policy::breadth_first()), view.end(policy::breadth_first()));
    vector<char> actualLeaves(view.begin(policy::leaves()), view.end(policy::leaves()));

    QCOMPARE(actualPreOrder, expectedPreOrder);
    QCOMPARE(actualPostOrder, expectedPostOrder);
    QCOMPARE(actualBreadthFirst, expectedBreadthFirst);
    QCOMPARE(actualLeaves, expectedLeaves);

    /*   ---   Backward order test   ---   */
    vector<char> actualReversePreOrder(view.rbegin(policy::pre_order()), view.rend(policy::pre_order()));
    vector<char> actualReversePostOrder(view.rbegin(policy::post_order()), view.rend(policy::post_order()));
    vector<char> actualReverseBreadthFirst(view.rbegin(policy::breadth_first()), view.rend(policy::breadth_first()));
    vector<char> actualReverseLeaves(view.rbegin(policy::leaves()), view.rend(policy::leaves()));

    vector<char> expectedReversePreOrder(expectedPreOrder.rbegin(), expectedPreOrder.rend());
    vector<char> expectedReversePostOrder(expectedPostOrder.rbegin(), expectedPostOrder.rend());
    vector<char> expectedReverseBreadthFirst(expectedBreadthFirst.rbegin(), expectedBreadthFirst.rend());
    vector<char> expectedReverseLeaves(expectedLeaves.rbegin(), expectedLeaves.rend());

    QCOMPARE(actualReversePreOrder, expectedReversePreOrder);
    QCOMPARE(actualReversePostOrder, expectedReversePostOrder);
    QCOMPARE(actualReverseBreadthFirst, expectedReverseBreadthFirst);
    QCOMPARE(actualReverseLeaves, expectedReverseLeaves);
}

QTEST_MAIN(NaryTreeViewIterationTest);
#include "NaryTreeViewIterationTest.moc"
