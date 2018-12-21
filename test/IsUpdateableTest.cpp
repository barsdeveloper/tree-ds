#include <QtTest/QtTest>

#include <TreeDS/tree>

#include "Types.hpp"

using namespace std;
using namespace md;

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
