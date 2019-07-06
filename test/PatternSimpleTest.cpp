#include <QtTest/QtTest>
#include <stdexcept>
#include <string>

#include <TreeDS/match>
#include <TreeDS/tree>

using namespace md;
using namespace std;

class PatternSimpleTest : public QObject {

    Q_OBJECT

    binary_tree<int> tree1 {
        n(1)(
            n(2),
            n(3))};
    binary_tree<int> tree2 {
        n(1)(
            n(2),
            n(1)(
                n(2),
                n(3)))};
    binary_tree<int> result;

    private slots:
    void construction();
    void test1();
    void test2();
    void test3();
    void test4();
    void test5();
    void test6();
    void test7();
    void test8();
};

void PatternSimpleTest::construction() {
    pattern p1(one());
    QCOMPARE(p1.mark_count(), 0);

    pattern p2(
        one(1)(
            one(2),
            one(3)));
    QCOMPARE(p2.mark_count(), 0);

    pattern p3(
        one('a')(
            cpt(one('b')),
            one('c')));
    QCOMPARE(p3.mark_count(), 1);

    pattern p4(
        cpt(
            one(string("alpha"))(
                cpt(one(string("beta"))),
                one(string("gamma")),
                cpt(one()))));
    QCOMPARE(p4.mark_count(), 3);

    pattern p5(
        cpt(cpt(
            star())));
    QCOMPARE(p5.mark_count(), 2);

    pattern p6(
        one()(
            cpt(star()(
                star()(
                    cpt(capture_name<'a', 'n', ' ', 'a'>(), one('a'))),
                cpt(one('b')(
                    cpt(star())))))));
    QCOMPARE(p6.mark_count(), 4);

    pattern p7(
        cpt(cpt(cpt(
            capture_name<'a'>(),
            star(string("string"))(
                cpt(capture_name<'t'>(), cpt(one())),
                cpt(one(string("b"))))))));
    QCOMPARE(p7.mark_count(), 6);
}

void PatternSimpleTest::test1() {
    pattern p(one(1));
    QVERIFY(p.match(tree1));
    p.assign_result(result);
    QCOMPARE(result, n(1));

    nary_tree<int> nary_int_result;
    QVERIFY_EXCEPTION_THROWN(p.assign_result(nary_int_result), std::invalid_argument);

    QVERIFY(p.match(tree2));
    p.assign_result(result);
    QCOMPARE(result, n(1));
}

void PatternSimpleTest::test2() {
    pattern p(
        one()(
            one(2)));
    QVERIFY(p.match(tree1));
    p.assign_result(result);
    QCOMPARE(result, n(1)(n(2)));

    QVERIFY(p.match(tree2));
    p.assign_result(result);
    QCOMPARE(result, n(1)(n(2)));
}

void PatternSimpleTest::test3() {
    pattern p {
        one(1)(
            one(3))};
    QVERIFY(p.match(tree1));
    p.assign_result(result);
    QCOMPARE(result, n(1)(n(), n(3)));

    QVERIFY(!p.match(tree2));
    p.assign_result(result);
    QCOMPARE(result, n());
}

void PatternSimpleTest::test4() {
    pattern p {
        cpt(one(1)(
            cpt(capture_name<'b'>(), one(2))))};
    QVERIFY(p.match(tree1));
    p.assign_result(result);
    QCOMPARE(result, n(1)(n(2)));
    p.assign_mark(capture_index<2>(), result);
    QCOMPARE(result, n(2));
    p.assign_mark(capture_index<1>(), result);
    QCOMPARE(result, n(1)(n(2)));
    p.assign_mark(capture_name<'b'>(), result);
    QCOMPARE(result, n(2));
}

void PatternSimpleTest::test5() {
    pattern p {
        one()(
            one(),
            one(3))};
    QVERIFY(p.match(tree1));
    p.assign_result(result);
    QCOMPARE(result, n(1)(n(2), n(3)));

    QVERIFY(!p.match(tree2));
    p.assign_result(result);
    QCOMPARE(result, n());
}

void PatternSimpleTest::test6() {
    pattern p {
        one(1)(
            one(2),
            cpt(capture_name<'t'>(), star<quantifier::RELUCTANT>()))};
    QVERIFY(p.match(tree1));
    p.assign_result(result);
    QCOMPARE(result, n(1)(n(2)));

    QVERIFY(p.match(tree2));
    p.assign_result(result);
    QCOMPARE(result, n(1)(n(2)));
}

void PatternSimpleTest::test7() {
    pattern p {
        one(1)(
            one(2),
            cpt(
                capture_name<'a'>(),
                star<quantifier::RELUCTANT>()(
                    one(3))))};
    QVERIFY(p.match(tree1));
    p.assign_result(result);
    QCOMPARE(result, n(1)(n(2), n(3)));

    QVERIFY(p.match(tree2));
    p.assign_result(result);
    QCOMPARE(
        result,
        n(1)(
            n(2),
            n(1)(
                n(),
                n(3))));
}

void PatternSimpleTest::test8() {
    pattern p {
        opt(1)(
            one(2),
            opt(3))};

    QVERIFY(p.match(tree2));
    //p.assign_result(result);
    //QCOMPARE(result, n(1)(n(2), n(3)));
}

QTEST_MAIN(PatternSimpleTest);
#include "PatternSimpleTest.moc"
