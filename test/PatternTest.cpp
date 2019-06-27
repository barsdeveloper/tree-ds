#include <QtTest/QtTest>
#include <stdexcept>
#include <string>

#include <TreeDS/match>
#include <TreeDS/tree>

using namespace md;
using namespace std;

class PatternTest : public QObject {

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
    binary_tree<char> tree3 {
        n('x')(
            n('a')(
                n('a')(
                    n(),
                    n('a')(
                        n('a')(
                            n('a'),
                            n('a')),
                        n('a')(
                            n('a')(
                                n(),
                                n('y')),
                            n('a')))),
                n('b')(
                    n('b'),
                    n('b')(
                        n('y')))),
            n('a'))};
    binary_tree<int> binary_int_result;
    binary_tree<char> binary_char_result;

    private slots:
    void construction();
    void simple1();
    void simple2();
    void simple3();
    void simple4();
    void simple5();
    void simple6();
    void simple7();
    void test1();
    void test2();
    void test3();
    void test4();
    void test5();
    void test6();
    void test7();
    void test8();
};

void PatternTest::construction() {
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

void PatternTest::simple1() {
    pattern p(one(1));
    QVERIFY(p.match(tree1));
    p.assign_result(binary_int_result);
    QCOMPARE(binary_int_result, n(1));

    nary_tree<int> nary_int_result;
    QVERIFY_EXCEPTION_THROWN(p.assign_result(nary_int_result), std::invalid_argument);

    QVERIFY(p.match(tree2));
    p.assign_result(binary_int_result);
    QCOMPARE(binary_int_result, n(1));
}

void PatternTest::simple2() {
    pattern p(
        one()(
            one(2)));
    QVERIFY(p.match(tree1));
    p.assign_result(binary_int_result);
    QCOMPARE(binary_int_result, n(1)(n(2)));

    QVERIFY(p.match(tree2));
    p.assign_result(binary_int_result);
    QCOMPARE(binary_int_result, n(1)(n(2)));
}

void PatternTest::simple3() {
    pattern p {
        one(1)(
            one(3))};
    QVERIFY(p.match(tree1));
    p.assign_result(binary_int_result);
    QCOMPARE(binary_int_result, n(1)(n(), n(3)));

    QVERIFY(!p.match(tree2));
    p.assign_result(binary_int_result);
    QCOMPARE(binary_int_result, n());
}

void PatternTest::simple4() {
    pattern p {
        cpt(one(1)(
            cpt(capture_name<'b'>(), one(2))))};
    QVERIFY(p.match(tree1));
    p.assign_result(binary_int_result);
    QCOMPARE(binary_int_result, n(1)(n(2)));
    p.assign_mark(capture_index<2>(), binary_int_result);
    QCOMPARE(binary_int_result, n(2));
    p.assign_mark(capture_index<1>(), binary_int_result);
    QCOMPARE(binary_int_result, n(1)(n(2)));
    p.assign_mark(capture_name<'b'>(), binary_int_result);
    QCOMPARE(binary_int_result, n(2));
}

void PatternTest::simple5() {
    pattern p {
        one()(
            one(),
            one(3))};
    QVERIFY(p.match(tree1));
    p.assign_result(binary_int_result);
    QCOMPARE(binary_int_result, n(1)(n(2), n(3)));

    QVERIFY(!p.match(tree2));
    p.assign_result(binary_int_result);
    QCOMPARE(binary_int_result, n());
}

void PatternTest::simple6() {
    pattern p {
        one(1)(
            one(2),
            cpt(capture_name<'t'>(), star<quantifier::RELUCTANT>()))};
    QVERIFY(p.match(tree1));
    p.assign_result(binary_int_result);
    QCOMPARE(binary_int_result, n(1)(n(2)));

    QVERIFY(p.match(tree2));
    p.assign_result(binary_int_result);
    QCOMPARE(binary_int_result, n(1)(n(2)));
}

void PatternTest::simple7() {
    pattern p {
        one(1)(
            one(2),
            cpt(
                capture_name<'a'>(),
                star<quantifier::RELUCTANT>()(
                    one(3))))};
    QVERIFY(p.match(tree1));
    p.assign_result(binary_int_result);
    QCOMPARE(binary_int_result, n(1)(n(2), n(3)));

    QVERIFY(p.match(tree2));
    p.assign_result(binary_int_result);
    QCOMPARE(
        binary_int_result,
        n(1)(
            n(2),
            n(1)(
                n(),
                n(3))));
}

void PatternTest::test1() {
    pattern p(
        star()(
            one('a'),
            star('b')(
                one('y'))));
    QVERIFY(p.match(tree3));
    p.assign_result(binary_char_result);
    QCOMPARE(
        binary_char_result,
        n('x')(
            n('a')(
                n('a'),
                n('b')(
                    n('b'),
                    n('b')(
                        n('y')))),
            n('a')));
}

void PatternTest::test2() {
    pattern p(
        one()(
            star<quantifier::RELUCTANT>('a')(
                one('y'),
                one('b'))));
    QVERIFY(p.match(tree3));
    p.assign_result(binary_char_result);
    QCOMPARE(
        binary_char_result,
        n('x')(
            n('a')(
                n('a')(
                    n(),
                    n('a')(
                        n(),
                        n('a')(
                            n('a')(
                                n(),
                                n('y'))))),
                n('b'))));
}

void PatternTest::test3() {
    pattern p(
        star()(
            cpt(capture_name<'P'>(),
                star('a')(
                    one('a')(
                        one('a'),
                        one('a')))),
            cpt(capture_name<'b'>(),
                star('b')(
                    cpt(capture_name<'y'>(),
                        star('y'))))));
    QVERIFY(p.match(tree3));
    p.assign_result(binary_char_result);
    QCOMPARE(
        binary_char_result,
        n('x')(
            n('a')(
                n('a')(
                    n(),
                    n('a')(
                        n('a'),
                        n('a'))),
                n('b')(
                    n('b'),
                    n('b')(
                        n('y')))),
            n('a')));
    p.assign_mark(capture_name<'P'>(), binary_char_result);
    QCOMPARE(
        binary_char_result,
        n('a')(
            n(),
            n('a')(
                n('a'),
                n('a'))));
    p.assign_mark(capture_name<'y'>(), binary_char_result);
    QCOMPARE(binary_char_result, n('y'));
}

void PatternTest::test4() {
    pattern p {
        star()(
            star(),
            star(),
            star())};
    QVERIFY(p.match(tree3));
    p.assign_result(binary_char_result);
    QCOMPARE(binary_char_result, n('x'));
}

void PatternTest::test5() {
    pattern p {
        star()(
            star(),
            one('x'),
            star())};
    QVERIFY(p.match(tree3));
    p.assign_result(binary_char_result);
    QCOMPARE(binary_char_result, n('x'));
}

void PatternTest::test6() {
    pattern p {
        star<quantifier::RELUCTANT>()(
            star(),
            one('x'),
            star())};
    QVERIFY(p.match(tree3));
    p.assign_result(binary_char_result);
    QCOMPARE(binary_char_result, n('x'));
}

void PatternTest::test7() {
    pattern p {
        star()(
            star(),
            star<quantifier::RELUCTANT>()(
                star(),
                one('x'),
                star()))};
    QVERIFY(p.match(tree3));
    p.assign_result(binary_char_result);
    QCOMPARE(binary_char_result, n('x'));
}

void PatternTest::test8() {
    // The only relevant node is one('x')
    pattern p {
        star()(
            star(),
            star<quantifier::GREEDY>(),
            star(),
            star(),
            star<quantifier::GREEDY>(),
            star(),
            star(),
            star<quantifier::RELUCTANT>()(
                star(),
                one('x')(
                    star<quantifier::RELUCTANT>()(
                        star<quantifier::GREEDY>()),
                    star<quantifier::RELUCTANT>(),
                    star<quantifier::RELUCTANT>(),
                    star<quantifier::RELUCTANT>()(
                        star<quantifier::RELUCTANT>(),
                        star<quantifier::GREEDY>()),
                    star<quantifier::RELUCTANT>()),
                star<quantifier::GREEDY>()),
            star(),
            star<quantifier::GREEDY>(),
            star(),
            star<quantifier::DEFAULT>())};
    QVERIFY(p.match(tree3));
    p.assign_result(binary_char_result);
    QCOMPARE(binary_char_result, n('x'));
}

QTEST_MAIN(PatternTest);
#include "PatternTest.moc"
