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
    void test1();
    void test2();
    void test3();
    void test4();
    void test5();
    void test6();
    void test7();
    void test8();
    void test9();
    void test10();
    void test11();
    void test12();
    void test13();
};

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
                        one('y'))))));
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
    QCOMPARE(binary_char_result, n('x')(n('a'), n('a')));
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

void PatternTest::test9() {
    pattern p {
        star('#')( // Not existing symbol
            one('x')(
                one('a'),
                one('a')))};
    QVERIFY(p.match(tree3));
}

void PatternTest::test10() {
    pattern p {
        opt<quantifier::POSSESSIVE>('x')(
            opt('y'),
            star('x')(
                star<quantifier::POSSESSIVE>('a')(
                    one('y'))))};
    QVERIFY(p.match(tree3));
    p.assign_result(binary_char_result);
    QCOMPARE(
        binary_char_result,
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
                            n('a')))))));
}

void PatternTest::test11() {
    pattern p {
        one('x')(
            opt<quantifier::GREEDY>('a'),
            star<quantifier::GREEDY>('a'),
            star<quantifier::POSSESSIVE>('a')(
                opt<quantifier::POSSESSIVE>('b')(
                    opt<quantifier::RELUCTANT>('b'),
                    opt('b')(
                        one('y')))))};
    QVERIFY(p.match(tree3));
    p.assign_result(binary_char_result);
    QCOMPARE(
        binary_char_result,
        n('x')(
            n('a')(
                n('a')(
                    n(),
                    n('a')(
                        n('a')(
                            n('a'),
                            n('a')),
                        n('a')(
                            n('a'),
                            n('a')))),
                n('b')(
                    n(),
                    n('b')(
                        n('y'))))));
}

void PatternTest::test12() {
    pattern p {
        one()(
            star<quantifier::POSSESSIVE>('a')(
                // Can't be matched because the parent possessive matcher will get every possible 'a', leaving no one
                one('a')))};
    QVERIFY(!p.match(tree3));
}

void PatternTest::test13() {
    pattern p {
        one()(
            star<quantifier::GREEDY>('a')(
                one('a')))};
    QVERIFY(p.match(tree3));
    print_tree(std::cout, tree3, {4, 100, true});
    std::cout << std::endl;
    p.assign_result(binary_char_result);
    QCOMPARE(
        binary_char_result,
        n('x')(
            n('a')(
                n('a'))));
}

QTEST_MAIN(PatternTest);
#include "PatternTest.moc"
