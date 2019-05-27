#include <QtTest>

#include <TreeDS/tree>
#include <TreeDS/view>

template class md::binary_tree_view<int>;
template class md::nary_tree_view<char>;

using namespace md;

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
    nary_tree_view<std::string> view(nary, std::find(nary.begin(), nary.end(), "four"));
    QCOMPARE(view.size(), 8);
    QCOMPARE(view.arity(), 4);
    QCOMPARE(*view.get_root(), "four");
    QCOMPARE(view.get_root().go_parent().get_raw_node(), nullptr); // Cannot go upper than the root using the iterator
    // TODO: Add more tests
}

QTEST_MAIN(TreeViewTest);
#include "TreeViewTest.moc"
