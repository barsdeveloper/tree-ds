#include <QtTest/QtTest>
#include <stdexcept>
#include <string>

#include <TreeDS/match>
#include <TreeDS/tree>

using namespace md;
using namespace std;

class PatternTest : public QObject {

    Q_OBJECT

    private slots:
    void construction();
    void simpleMatch();
};

void PatternTest::construction() {
    pattern p1(one(true_matcher()));
    QCOMPARE(p1.size(), 0);

    pattern p2(
        one(1)(
            one(2),
            one(3)));
    QCOMPARE(p2.size(), 0);

    pattern p3(
        one('a')(
            cpt(one('b')),
            one('c')));
    QCOMPARE(p3.size(), 1);

    pattern p4(
        cpt(
            one(string("alpha"))(
                cpt(one(string("beta"))),
                one(string("gamma")),
                cpt(one(true_matcher())))));
    QCOMPARE(p4.size(), 3);

    pattern p5(
        cpt(cpt(
            star(true_matcher()))));
    QCOMPARE(p5.size(), 2);

    pattern p6(
        one(true_matcher())(
            cpt(star(true_matcher())(
                star(true_matcher())(
                    cpt(capture_name<'a', 'n', ' ', 'a'>(), one('a'))),
                cpt(one('b')(
                    cpt(star(true_matcher()))))))));
    QCOMPARE(p6.size(), 4);

    pattern p7(
        cpt(cpt(cpt(
            capture_name<'a'>(),
            star(string("string"))(
                cpt(capture_name<'t'>(), cpt(one(true_matcher()))),
                cpt(one(string("b"))))))));
    QCOMPARE(p7.size(), 6);
}

void PatternTest::simpleMatch() {
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
    nary_tree<int> nary_result;

    pattern simple(one(1));
    QVERIFY(simple.match(tree1));
    simple.assign_result(result);
    QCOMPARE(result, n(1));
    QVERIFY_EXCEPTION_THROWN(simple.assign_result(nary_result), std::invalid_argument);

    QVERIFY(simple.match(tree2));
    simple.assign_result(result);
    QCOMPARE(result, n(1));

    pattern simple2(
        one(true_matcher())(
            one(2)));
    QVERIFY(simple2.match(tree1));
    simple2.assign_result(result);
    QCOMPARE(result, n(1)(n(2)));

    QVERIFY(simple2.match(tree2));
    simple2.assign_result(result);
    QCOMPARE(result, n(1)(n(2)));

    pattern simple3 {
        one(1)(
            one(3))};
    QVERIFY(simple3.match(tree1));
    simple3.assign_result(result);
    QCOMPARE(result, n(1)(n(), n(3)));

    QVERIFY(!simple3.match(tree2));
    simple3.assign_result(result);
    QCOMPARE(result, n());

    pattern simple4 {
        cpt(one(1)(
            cpt(capture_name<'b'>(), one(2))))};
    QVERIFY(simple4.match(tree1));
    simple4.assign_result(result);
    QCOMPARE(result, n(1)(n(2)));
    simple4.assign_capture(capture_index<1>(), result);
    QCOMPARE(result, n(2));
    simple4.assign_capture(capture_index<0>(), result);
    QCOMPARE(result, n(1)(n(2)));
    simple4.assign_capture(capture_name<'b'>(), result);
    QCOMPARE(result, n(2));

    pattern simple5 {
        one(true_matcher())(
            one(true_matcher()),
            one(3))};
    QVERIFY(simple5.match(tree1));
    simple5.assign_result(result);
    QCOMPARE(result, n(1)(n(2), n(3)));

    QVERIFY(!simple5.match(tree2));
    simple5.assign_result(result);
    QCOMPARE(result, n());

    pattern simple6 {
        one(1)(
            one(2),
            cpt(capture_name<'t'>(), star(true_matcher())))};
    QVERIFY(simple6.match(tree1));
    //simple5.assign_result(result);
    //QCOMPARE(result, n(1)(n(2), n(3)));

    QVERIFY(simple6.match(tree2));
}

QTEST_MAIN(PatternTest);
#include "PatternTest.moc"
