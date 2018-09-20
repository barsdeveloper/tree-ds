#include <QtTest/QtTest>

#include <TreeDS/tree.hpp>

#include "Types.hpp"

using namespace std;
using namespace ds;

Q_DECLARE_METATYPE(binary_tree<Foo>);
Q_DECLARE_METATYPE(nary_tree<Foo>);

class TreeTest : public QObject {

    Q_OBJECT

    private slots:
    void objectCreation() {

        nary_tree<Foo> tree;

        tree.emplace(tree.begin(), 93, 288);
    }
};

QTEST_MAIN(TreeTest);
#include "TreeTest.moc"
