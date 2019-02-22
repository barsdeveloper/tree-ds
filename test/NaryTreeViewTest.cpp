#include <QtTest/QtTest>

#include <TreeDS/tree>
#include <TreeDS/view>

using namespace md;

class NaryTreeViewTest : public QObject {

    Q_OBJECT

    private slots:
    void createFromNaryTree();
};

void NaryTreeViewTest::createFromNaryTree() {

    nary_tree<char> tree(
        n('a')(
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
                    n('r')))));
    nary_tree_view<char, post_order> view(tree);
    std::vector<char> actual(view.begin(), view.end());
    std::vector<char> expected {
        'b', 'f', 'k', 'l', 'g', '5', '3', '1', 'z', 'u', 's', 'm', 'n', 'o', 'p', '2', '0', 'v', 't', 'q', 'h', 'c',
        'i', 'd', 'r', 'j', 'e', 'a'};

    QCOMPARE(actual, expected);
    QCOMPARE(view.size(), 28);
    QCOMPARE(view.arity(), 5);

    nary_tree_view<char, breadth_first> view_c(tree, std::find(tree.begin(), tree.end(), 'c'));

    QCOMPARE(view_c.size(), 21);
    QCOMPARE(view_c.arity(), 5);

    actual.assign(view_c.begin(), view_c.end());
    expected = {
        'c', 'f', 'g', 'h', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 's', 't', 'u', 'v', 'z', '0', '1', '2', '3', '5'};
    QCOMPARE(actual, expected);

    actual.assign(view_c.begin(pre_order()), view_c.end(pre_order()));
    expected = {
        'c', 'f', 'g', 'k', 'l', 'h', 'm', 's', 'u', 'z', '1', '3', '5', 'n', 'o', 'p', 'q', 't', 'v', '0', '2'};
    QCOMPARE(actual, expected);

    std::reverse(actual.begin(), actual.end());
    std::reverse(expected.begin(), expected.end());
    QCOMPARE(actual, expected);

    actual.assign(view_c.begin(post_order()), view_c.end(post_order()));
    expected = {
        'f', 'k', 'l', 'g', '5', '3', '1', 'z', 'u', 's', 'm', 'n', 'o', 'p', '2', '0', 'v', 't', 'q', 'h', 'c'};
    QCOMPARE(actual, expected);

    std::reverse(actual.begin(), actual.end());
    std::reverse(expected.begin(), expected.end());
    QCOMPARE(actual, expected);
}

QTEST_MAIN(NaryTreeViewTest);
#include "NaryTreeViewTest.moc"
