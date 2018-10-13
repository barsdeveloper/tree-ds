#include <QtTest/QtTest>

#include <TreeDS/nary_tree>

#include "Types.hpp"

using namespace std;
using namespace ds;

class TreeTest : public QObject {

    Q_OBJECT

    using narytree_t = nary_tree<char, breadth_first<nary_node<char>>>;

    private slots:
    void naryTree();
    void binaryTree();
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

    std::vector<char> actual2(copy2.begin(), copy2.end());
    QCOMPARE(
        actual2,
        (std::vector<char>{'d', 'h', 'e', 'f', 'b', 'l', 'i', 'j', 'k', 'g', 'c', 'a'}));

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
        --std::find(tree.rbegin(), tree.rend(), 'g')
              .base(),
        '!');
    QCOMPARE(moved.size(), 8);
    QVERIFY(moved != copy2);
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
}

QTEST_MAIN(TreeTest);
#include "TreeTest.moc"
