#include <QtTest/QtTest>

#include <TreeDS/nary_tree>
#include <TreeDS/traversal_policy/in_order.hpp>

#include "Types.hpp"

using namespace std;
using namespace ds;

class TreeTest : public QObject {

    Q_OBJECT

    using narytree_t = nary_tree<char, breadth_first<nary_node<char>>>;

    private slots:
    void naryTree();
    void binaryTree();
    void nonCopyable();
};

void TreeTest::naryTree() {
    const narytree_t tree(
        n('a')(
            n('b')(
                n('d'),
                n('e')(
                    n('h')),
                n('f')),
            n('c')(
                n('g')(
                    n('i')(
                        n('l')),
                    n('j'),
                    n('k')))));
    std::vector<char> expected{'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l'};
    std::vector<char> actual(tree.begin(), tree.end());
    QCOMPARE(actual, expected);
    QCOMPARE(tree.size(), 12);

    narytree_t diff(tree);
    diff.insert(
        std::find(begin(diff), end(diff), 'h'),
        '#');
    actual.clear();
    for (const auto& value : diff) actual.push_back(value);
    expected[7] = '#';

    QCOMPARE(diff.size(), 12);
    QCOMPARE(actual, expected);
    QVERIFY(tree != diff);
    QVERIFY(diff != tree);
    QVERIFY(!(tree == diff));
    QVERIFY(!(diff == tree));

    narytree_t copy(tree);
    QVERIFY(copy == tree);
    QVERIFY(tree == copy);
    QVERIFY(!(copy != tree));
    QVERIFY(!(tree != copy));
    QVERIFY(copy != diff);
    QVERIFY(diff != copy);
    QVERIFY(!(copy == diff));
    QVERIFY(!(diff == copy));

    nary_tree<char, post_order> copy2(tree);
    QCOMPARE(copy2.size(), 12);
    QCOMPARE(copy, copy2);

    actual   = std::vector<char>(copy2.begin(), copy2.end());
    expected = std::vector<char>{'d', 'h', 'e', 'f', 'b', 'l', 'i', 'j', 'k', 'g', 'c', 'a'};
    QCOMPARE(actual, expected);

    nary_tree<char, pre_order> moved(std::move(copy2));
    QCOMPARE(moved.size(), 12);
    QCOMPARE(copy2.size(), 0);
    QCOMPARE(copy2.get_root(), nullptr);
    QVERIFY(moved != copy2);
    QVERIFY(moved == tree);
    QVERIFY(tree == moved);
    QVERIFY(!(moved != tree));
    QVERIFY(!(tree != moved));

    moved.insert(
        --std::find(moved.rbegin(), moved.rend(), 'g')
              .base(),
        '!');
    actual = std::vector<char>(
        moved.cbegin<breadth_first<nary_node<char>>>(),
        moved.cend<breadth_first<nary_node<char>>>());
    expected = std::vector<char>{'a', 'b', 'c', 'd', 'e', 'f', '!', 'h'};
    QCOMPARE(moved.size(), 8);
    QCOMPARE(actual, expected);
    QVERIFY(moved != copy2);

    auto it = std::find(
        moved.begin<breadth_first<nary_node<char>>>(),
        moved.end<breadth_first<nary_node<char>>>(),
        'b');
    *it    = '?';
    actual = std::vector<char>(
        moved.cbegin<breadth_first<nary_node<char>>>(),
        moved.cend<breadth_first<nary_node<char>>>());
    expected = std::vector<char>{'a', '?', 'c', 'd', 'e', 'f', '!', 'h'};

    QCOMPARE(moved.size(), 8);
    QCOMPARE(actual, expected);
}

void TreeTest::binaryTree() {
    const binary_tree<int, post_order> tree(
        n(-1)(
            n(),
            n(-2)(
                n(),
                n(-3)(
                    n(-4)(
                        n(-6)(
                            n(-10)),
                        n(-7)),
                    n(-5)(
                        n(-8)(
                            n(),
                            n(-11)),
                        n(-9)(
                            n(-12)))))));
    std::vector<int> expected{-10, -6, -7, -4, -11, -8, -12, -9, -5, -3, -2, -1};
    std::vector<int> actual(tree.begin(), tree.end());

    QCOMPARE(actual, expected);

    nary_tree<int, breadth_first<nary_node<int>>> nary;
    nary = tree;

    QCOMPARE(tree.size(), nary.size());
    QVERIFY(nary == tree);
    QVERIFY(!(nary != tree));
    QVERIFY(tree == nary);
    QVERIFY(!(tree != nary));
}

void TreeTest::nonCopyable() {
    binary_tree<NonCopyable> tree(
        n('a', 1)(
            n('b', 2),
            n('c', 3)(
                n('d', 4),
                n('e', 5))));
    // nary_tree<NonCopyable> anotherTree(tree); // this shouldn't compile
    binary_tree<NonCopyable> movedTree(std::move(tree));

    QVERIFY(tree.empty());
    QVERIFY(tree.size() == 0);
    QVERIFY(tree.begin() == tree.end());
    QVERIFY(!movedTree.empty());
    QVERIFY(movedTree.size() == 5);
    QVERIFY(movedTree != tree);
    QVERIFY(tree != movedTree);

    movedTree.emplace(
        std::find(movedTree.begin(), movedTree.end(), NonCopyable('c', 3)),
        'z', 100);
    auto it    = movedTree.begin<in_order>();
    auto itEnd = movedTree.end<in_order>();

    QCOMPARE(movedTree.size(), 3);
    QCOMPARE(*it++, NonCopyable('b', 2));
    QCOMPARE(*it++, NonCopyable('a', 1));
    QCOMPARE(*it++, NonCopyable('z', 100));
    QCOMPARE(it, itEnd);
}

QTEST_MAIN(TreeTest);
#include "TreeTest.moc"
