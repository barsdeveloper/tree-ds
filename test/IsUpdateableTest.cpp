#include <QtTest/QtTest>

#include <TreeDS/binary_tree>
#include <TreeDS/nary_tree>
#include <TreeDS/traversal_policy/breadth_first.hpp>
#include <TreeDS/traversal_policy/in_order.hpp>
#include <TreeDS/traversal_policy/post_order.hpp>
#include <TreeDS/traversal_policy/pre_order.hpp>
#include <TreeDS/utility.hpp>

#include "Types.hpp"

using namespace std;
using namespace ds;

class IsUpdateableTest : public QObject {

    Q_OBJECT

    private slots:
    void test() {
        QVERIFY((
            is_updateable<
                breadth_first<binary_node<int>>,
                binary_node<int>>::value));
        QVERIFY((
            !is_updateable<
                breadth_first<binary_node<int>>,
                binary_node<float>>::value));
        QVERIFY((
            is_updateable<
                breadth_first<binary_node<Foo>>,
                binary_node<Foo>>::value));
        QVERIFY((
            !is_updateable<
                breadth_first<binary_node<Foo>>,
                binary_node<Bar>>::value));
        QVERIFY((
            is_updateable<
                breadth_first<nary_node<string>>,
                nary_node<string>>::value));
        QVERIFY((
            !is_updateable<
                breadth_first<nary_node<string>>,
                nary_node<char>>::value));
        QVERIFY((
            is_updateable<
                breadth_first<nary_node<Bar>>,
                nary_node<Bar>>::value));
    }
};

QTEST_MAIN(IsUpdateableTest);
#include "IsUpdateableTest.moc"
