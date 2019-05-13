#include <QtTest/QtTest>
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
    pattern p1(one(true_match));
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
                cpt(one(true_match)))));
    QCOMPARE(p4.size(), 3);

    pattern p5(
        cpt(cpt(
            star())));
    QCOMPARE(p5.size(), 2);

    pattern p6(
        one(true_match)(
            cpt(star()(
                star()(
                    cpt(capture_name<'a', 'n', ' ', 'a'>(), one('a'))),
                cpt(one('b')(
                    cpt(star())))))));
    QCOMPARE(p6.size(), 4);

    pattern p7(
        cpt(cpt(cpt(
            capture_name<'a'>(),
            star(string("string"))(
                cpt(capture_name<'a'>(), cpt(one(true_match))),
                cpt(one(string("b"))))))));
    QCOMPARE(p7.size(), 6);
}

void PatternTest::simpleMatch() {
    binary_tree<int> tree {
        n(1)(
            n(2),
            n(3))};
    binary_tree<int> result;

    pattern simple(one(1));
    QVERIFY(simple.match(tree));
    simple.assign_result(result);
    QCOMPARE(result, n(1));

    pattern simple2(
        one(true_match)(
            one(2)));
    QVERIFY(simple2.match(tree));
    simple2.assign_result(result);
    QCOMPARE(result, n(1)(n(2)));

    pattern simple3 {
        cpt(one(1)(
            cpt(capture_name<'b'>(), one(2))))};
    QVERIFY(simple3.match(tree));
    simple3.assign_result(result);
    QCOMPARE(result, n(1)(n(2)));
    simple3.assign_capture<1>(result);
    QCOMPARE(result, n(2));
    simple3.assign_capture<0>(result);
    QCOMPARE(result, n(1)(n(2)));

    pattern simple4 {
        one(true_match)(
            one(true_match),
            one(3))};
    QVERIFY(simple4.match(tree));
    simple4.assign_result(result);
    QCOMPARE(result, n(1)(n(2), n(3)));
}

QTEST_MAIN(PatternTest);
#include "PatternTest.moc"
