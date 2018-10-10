#include <QtTest/QtTest>

#include <TreeDS/nary_tree>

#include "Types.hpp"

using namespace std;
using namespace ds;

class TreeTest : public QObject {

    Q_OBJECT

    using tree_t = nary_tree<char, breadth_first<nary_node<char>>>;

    private slots:
    void naryTree() {
        const tree_t tree(
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
        std::vector<char> actual;
        for (const auto& value : tree) actual.push_back(value);
        QCOMPARE(actual, expected);

        tree_t diff(tree);
        diff.insert(
            std::find(begin(diff), end(diff), 'h'),
            '#');
        actual.clear();
        for (const auto& value : diff) actual.push_back(value);
        expected[7] = '#';
        QCOMPARE(actual, expected);
        QVERIFY(tree != diff);

        tree_t copy(tree);
        QCOMPARE(copy, tree);

        nary_tree<char, post_order> copy2(tree);
        QCOMPARE(copy, copy2);

        std::vector<char> actual2;
        for (const auto& value : copy2) actual2.push_back(value);
        QCOMPARE(
            actual2,
            (std::vector<char>{'d', 'h', 'e', 'f', 'b', 'l', 'i', 'j', 'k', 'g', 'c', 'a'}));
    }
};

QTEST_MAIN(TreeTest);
#include "TreeTest.moc"
