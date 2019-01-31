#include <QtTest/QtTest>
#include <stdexcept> // std::logic_error

#include <TreeDS/tree>

#include "Types.hpp"

using namespace std;
using namespace md;

template class md::tree<int, nary_node<int>, pre_order, std::allocator<int>>;

class TreeTest : public QObject {

    Q_OBJECT

    using narytree_t = nary_tree<char, breadth_first>;

    private slots:
    void naryTree();
    void binaryTree();
    void nonCopyable();
    void forbiddenOperations();
};

void TreeTest::naryTree() {
    auto struct_tree = n('a')(
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
                n('k'))));
    const narytree_t tree(struct_tree);
    QVERIFY(tree == struct_tree);
    QVERIFY(struct_tree == tree);
    QVERIFY(!(tree != struct_tree));
    QVERIFY(!(struct_tree != tree));
    QVERIFY((
        tree
        != n('a')(
               n('b')(
                   n('d'),
                   n('e')(
                       n('h')),
                   n('f')),
               n('c')(
                   n('g')(
                       n('i')(
                           n('l')),
                       n('j')))))); // no 'k'

    vector<char> expected {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l'};
    vector<char> actual(tree.begin(), tree.end());
    QCOMPARE(actual, expected);
    QCOMPARE(tree.size(), 12);
    QCOMPARE(tree.arity(), 3);

    // h -> #
    narytree_t diff(tree);
    auto sharp_it = diff.insert(
        find(begin(diff), end(diff), 'h'),
        '#');
    QCOMPARE(*sharp_it, '#');

    actual.clear();
    for (const auto& value : diff) actual.push_back(value);
    expected[7] = '#';

    QCOMPARE(diff.size(), 12);
    QCOMPARE(diff.arity(), 3);
    QCOMPARE(actual, expected);
    QVERIFY(tree != diff);
    QVERIFY(diff != tree);
    QVERIFY(!(tree == diff));
    QVERIFY(!(diff == tree));

    narytree_t copy;
    copy = tree;
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
    QCOMPARE(copy2.arity(), 3);
    QCOMPARE(copy, copy2);

    actual   = vector<char>(copy2.begin(), copy2.end());
    expected = vector<char> {'d', 'h', 'e', 'f', 'b', 'l', 'i', 'j', 'k', 'g', 'c', 'a'};
    QCOMPARE(actual, expected);

    nary_tree<char, post_order> moved(move(copy2));
    QCOMPARE(moved.size(), 12);
    QCOMPARE(moved.arity(), 3);
    QCOMPARE(copy2.size(), 0);
    QCOMPARE(copy2.arity(), 0);
    QCOMPARE(copy2.get_root(), nullptr);
    QVERIFY(moved != copy2);
    QVERIFY(moved == tree);
    QVERIFY(tree == moved);
    QVERIFY(!(moved != tree));
    QVERIFY(!(tree != moved));

    copy2 = move(moved);
    QCOMPARE(copy2.size(), 12);
    QCOMPARE(copy2.arity(), 3);
    QCOMPARE(moved.size(), 0);
    QCOMPARE(moved.arity(), 0);
    QVERIFY(moved.get_root() == nullptr);
    QVERIFY(copy2.get_root() != nullptr);

    moved = move(copy2);
    QCOMPARE(copy2.size(), 0);
    QCOMPARE(copy2.arity(), 0);
    QCOMPARE(moved.size(), 12);
    QCOMPARE(moved.arity(), 3);
    QVERIFY(moved.get_root() != nullptr);
    QVERIFY(copy2.get_root() == nullptr);

    // g -> !
    auto bang_it = moved.insert(
        --find(moved.rbegin(), moved.rend(), 'g')
              .base(),
        '!');
    QCOMPARE(*bang_it, '!');

    actual = vector<char>(
        moved.cbegin<breadth_first>(),
        moved.cend<breadth_first>());
    expected = vector<char> {'a', 'b', 'c', 'd', 'e', 'f', '!', 'h'};
    QCOMPARE(moved.size(), 8);
    QCOMPARE(moved.arity(), 3);
    QCOMPARE(actual, expected);
    QVERIFY(moved != copy2);

    // b -> ?
    auto it = find(
        moved.begin<breadth_first>(),
        moved.end<breadth_first>(),
        'b');
    *it    = '?';
    actual = vector<char>(
        moved.cbegin<breadth_first>(),
        moved.cend<breadth_first>());
    expected = vector<char> {'a', '?', 'c', 'd', 'e', 'f', '!', 'h'};

    QCOMPARE(moved.size(), 8);
    QCOMPARE(moved.arity(), 3);
    QCOMPARE(actual, expected);

    // e -> t
    auto t_it = moved.insert(
        find(moved.begin(), moved.end(), 'e'),
        't');
    QCOMPARE(
        find(moved.begin(), moved.end(), 't').get_node()->get_parent()->get_value(),
        '?');
    QCOMPARE(*t_it, 't');

    // erase root
    auto end_it = moved.erase(find(moved.begin<post_order>(), moved.end<post_order>(), 'a'));
    QVERIFY(end_it == moved.end<post_order>());

    QCOMPARE(moved, n());
    QCOMPARE(moved.size(), 0);
    QCOMPARE(moved.arity(), 0);
    QCOMPARE(moved.begin(), moved.end());
    QCOMPARE(moved.begin<post_order>(), end_it);
    QVERIFY(moved.empty());
    QVERIFY(moved == n());
    QVERIFY(n() == moved);
    QVERIFY(!(moved != n()));
    QVERIFY(!(n() != moved));
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
    vector<int> expected {-10, -6, -7, -4, -11, -8, -12, -9, -5, -3, -2, -1};
    vector<int> actual(tree.begin(), tree.end());
    QCOMPARE(tree.size(), 12);
    QCOMPARE(tree.arity(), 2);
    QCOMPARE(actual, expected);
    QVERIFY((tree != binary_tree<int, breadth_first>()));
    QVERIFY((binary_tree<int, breadth_first>() != tree));

    binary_tree<int, pre_order> copy;
    copy = tree;
    QVERIFY(copy == tree);
    QVERIFY(!(copy != tree));
    QVERIFY(tree == copy);
    QVERIFY(!(tree != copy));

    nary_tree<int, breadth_first> nary;
    nary = tree;

    QCOMPARE(tree.size(), nary.size());
    QCOMPARE(tree.arity(), nary.arity());
    QVERIFY(nary == tree);
    QVERIFY(!(nary != tree));
    QVERIFY(tree == nary);
    QVERIFY(!(tree != nary));

    binary_tree<int> tree2(tree);
    auto eight_it = tree2.erase(std::find(tree2.begin<post_order>(), tree2.end<post_order>(), -11));
    QCOMPARE(*eight_it, -8);

    QCOMPARE(tree2.size(), nary.size() - 1);
    QVERIFY(nary != tree2);
    QVERIFY(tree2 != nary);

    auto end_it = tree2.erase(
        tree2.begin<post_order>(),
        tree2.end<post_order>());
    QCOMPARE(end_it, tree2.end<post_order>());

    QVERIFY(tree2.empty());
}

void TreeTest::nonCopyable() {
    binary_tree<NonCopyable> tree(
        n('a', 1)(
            n('b', 2),
            n('c', 3)(
                n('d', 4),
                n('e', 5))));
    // nary_tree<NonCopyable> anotherTree(tree); // this shouldn't compile
    binary_tree<NonCopyable> movedTree(move(tree));

    QVERIFY(tree.empty());
    QCOMPARE(tree.size(), 0);
    QCOMPARE(tree.arity(), 0);
    QVERIFY(tree.begin() == tree.end());
    QVERIFY(!movedTree.empty());
    QCOMPARE(movedTree.size(), 5);
    QCOMPARE(movedTree.arity(), 2);
    QVERIFY(movedTree != tree);
    QVERIFY(tree != movedTree);

    // (c, 3) -> (z, 100)
    auto z_it = movedTree.emplace(
        find(movedTree.begin(), movedTree.end(), NonCopyable('c', 3)),
        'z', 100);
    QCOMPARE(*z_it, NonCopyable('z', 100));

    auto it    = movedTree.begin<in_order>();
    auto itEnd = movedTree.end<in_order>();

    QCOMPARE(movedTree.size(), 3);
    QCOMPARE(movedTree.arity(), 2);
    QCOMPARE(*it++, NonCopyable('b', 2));
    QCOMPARE(*it++, NonCopyable('a', 1));
    QCOMPARE(*it++, NonCopyable('z', 100));
    QCOMPARE(it, itEnd);
}

void TreeTest::forbiddenOperations() {
    binary_tree<std::string> binEmpty;
    binary_tree<std::string> bin(
        n("a")(
            n("b")(
                n(),
                n("d")(
                    n("e"),
                    n("f"))),
            n("c")));

    QVERIFY(bin != binEmpty);
    QVERIFY(binEmpty != bin);
    QVERIFY(!(bin == binEmpty));
    QVERIFY(!(binEmpty == bin));

    // iterator belonging to another tree
    QVERIFY_EXCEPTION_THROWN(
        binEmpty.emplace(bin.begin(), "x"),
        std::logic_error);
    QVERIFY_EXCEPTION_THROWN(
        bin.insert(binEmpty.begin(), std::string("x")),
        std::logic_error);
    QVERIFY_EXCEPTION_THROWN(
        bin.erase(binEmpty.begin<post_order>()),
        std::logic_error);
    // iterator pointing to end
    QVERIFY_EXCEPTION_THROWN(
        bin.emplace(bin.end(), "x"),
        std::logic_error);
    QVERIFY_EXCEPTION_THROWN(
        bin.erase(bin.end<post_order>()),
        std::logic_error);
    QVERIFY_EXCEPTION_THROWN(
        binEmpty.erase(bin.begin<post_order>(), bin.end<post_order>()),
        std::logic_error);
}

QTEST_MAIN(TreeTest);
#include "TreeTest.moc"
