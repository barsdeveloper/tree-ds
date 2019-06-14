#include <QtTest/QtTest>
#include <sstream> // std::stringstream

#include <TreeDS/tree>

#include "Types.hpp"

using namespace std;
using namespace md;

template class md::tree<nary_node<int>, policy::breadth_first, std::allocator<int>>;

class TreeTest : public QObject {

    Q_OBJECT

    using narytree_t = nary_tree<char, policy::breadth_first>;

    private slots:
    void naryTree();
    void naryTree2();
    void binaryTree();
    void nonCopyable();
    void forbiddenOperations();
};

void TreeTest::naryTree() {
    auto struct_tree
        = n('a')(
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
                    n('j')))))); // No 'k'
    stringstream stream;
    stream << tree;
    QCOMPARE(
        stream.str(),
        string(
            "n('a')(\n"
            "    n('b')(\n"
            "        n('d'),\n"
            "        n('e')(\n"
            "            n('h')),\n"
            "        n('f')),\n"
            "    n('c')(\n"
            "        n('g')(\n"
            "            n('i')(\n"
            "                n('l')),\n"
            "            ...)))"));

    vector<char> expected {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l'};
    vector<char> actual(tree.begin(), tree.end());
    QCOMPARE(actual, expected);
    QCOMPARE(tree.size(), 12);
    QCOMPARE(tree.arity(), 3);

    // h -> #
    narytree_t diff(tree);
    auto sharp_it = diff.insert_over(
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

    nary_tree<char, policy::post_order> copy2(tree);
    QCOMPARE(copy2.size(), 12);
    QCOMPARE(copy2.arity(), 3);
    QCOMPARE(copy, copy2);

    actual   = vector<char>(copy2.begin(), copy2.end());
    expected = vector<char> {'d', 'h', 'e', 'f', 'b', 'l', 'i', 'j', 'k', 'g', 'c', 'a'};
    QCOMPARE(actual, expected);

    nary_tree<char, policy::post_order> moved(move(copy2));
    QCOMPARE(moved.size(), 12);
    QCOMPARE(moved.arity(), 3);
    QCOMPARE(copy2.size(), 0);
    QCOMPARE(copy2.arity(), 0);
    QCOMPARE(copy2.get_root(), copy2.end(policy::fixed()));
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
    QVERIFY(moved.get_raw_root() == nullptr);
    QCOMPARE(*copy2.get_root(), 'a');

    moved = move(copy2);
    QCOMPARE(copy2.size(), 0);
    QCOMPARE(copy2.arity(), 0);
    QCOMPARE(moved.size(), 12);
    QCOMPARE(moved.arity(), 3);
    QCOMPARE(*moved.get_root(), 'a');
    QVERIFY(copy2.get_raw_root() == nullptr);

    // g -> !
    auto bang_it = moved.insert_over(
        --find(moved.rbegin(), moved.rend(), 'g')
              .base(),
        '!');
    QCOMPARE(moved.size(), 8);
    QCOMPARE(moved.arity(), 3);
    QCOMPARE(*bang_it, '!');
    actual = vector<char>(
        moved.cbegin(policy::breadth_first()),
        moved.cend(policy::breadth_first()));
    expected = vector<char> {'a', 'b', 'c', 'd', 'e', 'f', '!', 'h'};
    QCOMPARE(actual, expected);
    QVERIFY(moved != copy2);
    QCOMPARE(bang_it.get_raw_node()->get_following_siblings(), 0);

    // b -> ?
    auto it = find(
        moved.begin(policy::breadth_first()),
        moved.end(policy::breadth_first()),
        'b');
    *it    = '?';
    actual = vector<char>(
        moved.cbegin(policy::breadth_first()),
        moved.cend(policy::breadth_first()));
    expected = vector<char> {'a', '?', 'c', 'd', 'e', 'f', '!', 'h'};
    QCOMPARE(moved.size(), 8);
    QCOMPARE(moved.arity(), 3);
    QCOMPARE(actual, expected);
    QCOMPARE(it.get_raw_node()->get_following_siblings(), 1);

    // e -> t
    auto t_it = moved.insert_over(
        find(moved.begin(), moved.end(), 'e'),
        't');
    // Parent of t is now ?
    QCOMPARE(
        find(moved.begin(), moved.end(), 't').get_raw_node()->get_parent()->get_value(),
        '?');
    QCOMPARE(*t_it, 't');

    nary_node<char>* n_d = find(moved.begin(), moved.end(), 'd').get_raw_node();
    nary_node<char>* n_t = find(moved.begin(), moved.end(), 't').get_raw_node();
    nary_node<char>* n_f = find(moved.begin(), moved.end(), 'f').get_raw_node();
    QCOMPARE(n_d->get_prev_sibling(), nullptr);
    QCOMPARE(n_d->get_next_sibling(), n_t);
    QCOMPARE(n_d->get_following_siblings(), 2);
    QCOMPARE(n_t->get_prev_sibling(), n_d);
    QCOMPARE(n_t->get_next_sibling(), n_f);
    QCOMPARE(n_t->get_following_siblings(), 1);
    QCOMPARE(n_f->get_prev_sibling(), n_t);
    QCOMPARE(n_f->get_next_sibling(), nullptr);
    QCOMPARE(n_f->get_following_siblings(), 0);

    // Erase t
    auto it_f = moved.erase(find(moved.begin(policy::post_order()), moved.end(policy::post_order()), 't'));
    QCOMPARE(it_f.get_raw_node(), n_f);
    QCOMPARE(n_d->get_prev_sibling(), nullptr);
    QCOMPARE(n_d->get_next_sibling(), n_f);
    QCOMPARE(n_d->get_following_siblings(), 1);
    QCOMPARE(n_f->get_prev_sibling(), n_d);
    QCOMPARE(n_f->get_next_sibling(), nullptr);
    QCOMPARE(n_f->get_following_siblings(), 0);

    // Erase

    // Erase root
    auto end_it
        = moved.erase(find(moved.begin(policy::post_order()), moved.end(policy::post_order()), 'a'));
    QVERIFY(end_it == moved.end(policy::post_order()));
    QCOMPARE(moved, n());
    QCOMPARE(moved.size(), 0);
    QCOMPARE(moved.arity(), 0);
    QCOMPARE(moved.begin(), moved.end());
    QCOMPARE(moved.begin(policy::post_order()), end_it);
    QVERIFY(moved.empty());
    QVERIFY(moved == n());
    QVERIFY(n() == moved);
    QVERIFY(!(moved != n()));
    QVERIFY(!(n() != moved));
}

void TreeTest::naryTree2() {
    nary_tree<Foo> tree;

    tree.emplace_over(tree.begin(), 0, 0);
    QCOMPARE(tree.size(), 1);
    QCOMPARE(tree.arity(), 0);

    const nary_node<Foo>* root = tree.get_root().get_raw_node();
    QCOMPARE(root->get_parent(), nullptr);
    QCOMPARE(root->get_following_siblings(), 0);
    QCOMPARE(root->get_prev_sibling(), nullptr);
    QCOMPARE(root->get_next_sibling(), nullptr);
    QCOMPARE(root->get_first_child(), nullptr);
    QCOMPARE(root->get_last_child(), nullptr);

    tree.insert_child_front(tree.get_root(), Foo(3, 6));
    tree.insert_child_back(tree.get_root(), Foo(4, 7));
    tree.emplace_child_front(tree.get_root(), 2, 5);
    QCOMPARE(tree.size(), 4);
    QCOMPARE(tree.arity(), 3);

    auto siblings_it(tree.get_root().go_first_child().other_policy(policy::siblings()));
    QCOMPARE(*siblings_it, Foo(2, 5));
    QCOMPARE(siblings_it.get_raw_node()->get_following_siblings(), 2);
    QCOMPARE(siblings_it.get_raw_node()->get_prev_sibling(), nullptr);
    QCOMPARE(siblings_it.get_raw_node()->get_next_sibling()->get_value(), Foo(3, 6));
    ++siblings_it;
    QCOMPARE(*siblings_it, Foo(3, 6));
    QCOMPARE(siblings_it.get_raw_node()->get_following_siblings(), 1);
    QCOMPARE(siblings_it.get_raw_node()->get_prev_sibling()->get_value(), Foo(2, 5));
    QCOMPARE(siblings_it.get_raw_node()->get_next_sibling()->get_value(), Foo(4, 7));
    ++siblings_it;
    QCOMPARE(*siblings_it, Foo(4, 7));
    QCOMPARE(siblings_it.get_raw_node()->get_following_siblings(), 0);
    QCOMPARE(siblings_it.get_raw_node()->get_prev_sibling()->get_value(), Foo(3, 6));
    QCOMPARE(siblings_it.get_raw_node()->get_next_sibling(), nullptr);

    tree.erase(tree.get_root().go_child(1).other_policy(policy::post_order()));
    QCOMPARE(tree.size(), 3);
    QCOMPARE(tree.arity(), 2);

    siblings_it = tree.get_root().go_first_child().other_policy(policy::siblings());
    QCOMPARE(*siblings_it, Foo(2, 5));
    QCOMPARE(siblings_it.get_raw_node()->get_following_siblings(), 1);
    QCOMPARE(siblings_it.get_raw_node()->get_prev_sibling(), nullptr);
    QCOMPARE(siblings_it.get_raw_node()->get_next_sibling()->get_value(), Foo(4, 7));
    ++siblings_it;
    QCOMPARE(*siblings_it, Foo(4, 7));
    QCOMPARE(siblings_it.get_raw_node()->get_following_siblings(), 0);
    QCOMPARE(siblings_it.get_raw_node()->get_prev_sibling()->get_value(), Foo(2, 5));
    QCOMPARE(siblings_it.get_raw_node()->get_next_sibling(), nullptr);

    tree.emplace_child_back(siblings_it, 10, 11);
    QCOMPARE(tree.size(), 4);
    QCOMPARE(tree.arity(), 2);
    QCOMPARE(
        tree,
        n(Foo(0, 0))(
            n(Foo(2, 5)),
            n(Foo(4, 7))(
                n(Foo(10, 11)))));

    tree.emplace_over(
        siblings_it,
        n(-1, -2)(
            n(-2, -3),
            n(-3, -4)(
                n(-7, -8)),
            n(-4, -5),
            n(-5, -6)));
    QCOMPARE(tree.size(), 8);
    QCOMPARE(tree.arity(), 4);

    auto it = std::find(tree.begin(), tree.end(), Foo(-2, -3)).other_policy(policy::siblings());
    QCOMPARE(it.get_raw_node()->get_following_siblings(), 3);
    QCOMPARE(it.get_raw_node()->get_prev_sibling(), nullptr);
    QCOMPARE(it.get_raw_node()->get_next_sibling()->get_value(), Foo(-3, -4));
    ++it;
    QCOMPARE(*it, Foo(-3, -4));
    QCOMPARE(it.get_raw_node()->get_following_siblings(), 2);
    QCOMPARE(it.get_raw_node()->get_prev_sibling()->get_value(), Foo(-2, -3));
    QCOMPARE(it.get_raw_node()->get_next_sibling()->get_value(), Foo(-4, -5));
    ++it;
    QCOMPARE(*it, Foo(-4, -5));
    QCOMPARE(it.get_raw_node()->get_following_siblings(), 1);
    QCOMPARE(it.get_raw_node()->get_prev_sibling()->get_value(), Foo(-3, -4));
    QCOMPARE(it.get_raw_node()->get_next_sibling()->get_value(), Foo(-5, -6));
    ++it;
    QCOMPARE(*it, Foo(-5, -6));
    QCOMPARE(it.get_raw_node()->get_following_siblings(), 0);
    QCOMPARE(it.get_raw_node()->get_prev_sibling()->get_value(), Foo(-4, -5));
    QCOMPARE(it.get_raw_node()->get_next_sibling(), nullptr);
}

void TreeTest::binaryTree() {
    const binary_tree<int, policy::post_order> tree(
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
    QVERIFY((tree != binary_tree<int, policy::breadth_first>()));
    QVERIFY((binary_tree<int, policy::breadth_first>() != tree));

    binary_tree<int, policy::pre_order> copy;
    copy = tree;
    QVERIFY(copy == tree);
    QVERIFY(!(copy != tree));
    QVERIFY(tree == copy);
    QVERIFY(!(tree != copy));

    nary_tree<int, policy::breadth_first> nary;
    nary = tree;

    QCOMPARE(tree.size(), nary.size());
    QCOMPARE(tree.arity(), nary.arity());
    QVERIFY(nary == tree);
    QVERIFY(!(nary != tree));
    QVERIFY(tree == nary);
    QVERIFY(!(tree != nary));

    binary_tree<int> tree2(tree);
    auto eight_it = tree2.erase(std::find(tree2.begin(policy::post_order()), tree2.end(policy::post_order()), -11));
    QCOMPARE(*eight_it, -8);

    QCOMPARE(tree2.size(), nary.size() - 1);
    QVERIFY(nary != tree2);
    QVERIFY(tree2 != nary);

    auto end_it = tree2.erase(
        tree2.begin(policy::post_order()),
        tree2.end(policy::post_order()));
    QCOMPARE(end_it, tree2.end(policy::post_order()));

    QVERIFY(tree2.empty());
}

void TreeTest::nonCopyable() {
    binary_tree<NonCopyable> tree(
        n('a', 1)(
            n('b', 2),
            n('c', 3)(
                n('d', 4),
                n('e', 5))));
    // nary_tree<NonCopyable> anotherTree(tree); // This shouldn't compile.
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
    auto z_it = movedTree.emplace_over(
        find(movedTree.begin(), movedTree.end(), NonCopyable('c', 3)),
        'z', 100);
    QCOMPARE(*z_it, NonCopyable('z', 100));

    auto it    = movedTree.begin(policy::in_order());
    auto itEnd = movedTree.end(policy::in_order());

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

    // Iterator belonging to another tree.
    QVERIFY_EXCEPTION_THROWN(
        binEmpty.emplace_over(bin.begin(), "x"),
        std::logic_error);
    QVERIFY_EXCEPTION_THROWN(
        bin.insert_over(binEmpty.begin(), std::string("x")),
        std::logic_error);
    QVERIFY_EXCEPTION_THROWN(
        bin.erase(binEmpty.begin(policy::post_order())),
        std::logic_error);
    // Iterator pointing to end.
    QVERIFY_EXCEPTION_THROWN(
        bin.emplace_over(bin.end(), "x"),
        std::logic_error);
    QVERIFY_EXCEPTION_THROWN(
        bin.erase(bin.end(policy::post_order())),
        std::logic_error);
    QVERIFY_EXCEPTION_THROWN(
        binEmpty.erase(bin.begin(policy::post_order()), bin.end(policy::post_order())),
        std::logic_error);
}

QTEST_MAIN(TreeTest);
#include "TreeTest.moc"
