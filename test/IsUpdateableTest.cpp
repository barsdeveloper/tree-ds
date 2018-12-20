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
                breadth_first<binary::node<int>>,
                binary::node<int>>::value));

        QVERIFY((
            !is_updateable<
                breadth_first<binary::node<int>>,
                binary::node<float>>::value));

        QVERIFY((
            is_updateable<
                breadth_first<binary::node<Foo>>,
                binary::node<Foo>>::value));

        QVERIFY((
            !is_updateable<
                breadth_first<binary::node<Foo>>,
                binary::node<Bar>>::value));

        QVERIFY((
            is_updateable<
                breadth_first<nary::node<string>>,
                nary::node<string>>::value));

        QVERIFY((
            !is_updateable<
                breadth_first<nary::node<string>>,
                nary::node<char>>::value));

        QVERIFY((
            is_updateable<
                breadth_first<nary::node<Bar>>,
                nary::node<Bar>>::value));
    }
};

QTEST_MAIN(IsUpdateableTest);
#include "IsUpdateableTest.moc"
