#include <QtTest>

#include <TreeDS/tree>
#include <TreeDS/view>

template class md::binary_tree_view<int>;
template class md::nary_tree_view<char>;

using namespace md;
using namespace std;

class TreeViewTest : public QObject {

    Q_OBJECT

    const nary_tree<std::string> nary {
        n("one")(
            n("two")(
                n("five")),
            n("three"),
            n("four")(
                n("six")(
                    n("ten")),
                n("seven")(
                    n("eleven"),
                    n("twelve")),
                n("eight"),
                n("nine")))};

    private slots:
    void naryTreeView();
};

void TreeViewTest::naryTreeView() {
    nary_tree_view<string> view1(nary, find(nary.begin(), nary.end(), "four"));
    QCOMPARE(view1.size(), 8);
    QCOMPARE(view1.arity(), 4);
    QCOMPARE(*view1.root(), "four");
    QCOMPARE(view1.root().go_parent().get_raw_node(), nullptr); // Cannot go upper than the root using the iterator

    nary_tree_view<string> view2;
    view2 = nary;
    QCOMPARE(view2.size(), 12);
    QCOMPARE(view2.arity(), 4);
    QCOMPARE(*view2.root(), "one");

    view2 = nary_tree_view<string> {view2, view2.root().go_first_child().go_first_child()};
}

QTEST_MAIN(TreeViewTest);
#include "TreeViewTest.moc"
