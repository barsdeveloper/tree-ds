#include <QtTest/QtTest>
#include <iostream>

#define MD_PRINT_TREE_ADDRESS_DIGITS 4
#define MD_PRINT_TREE_MAX_NODES 1000

#include <TreeDS/match>
#include <TreeDS/tree>

using namespace md;
using namespace std;

class PatternTest2 : public QObject {

    Q_OBJECT

    nary_tree<char> tree {
        n('a')(
            n('a')(
                n('a')(
                    n('b')(
                        n('a'),
                        n('a')(
                            n('b')(
                                n('b')))),
                    n('b'),
                    n('a')),
                n('b'),
                n('b')(
                    n('a'),
                    n('a'),
                    n('b')(
                        n('a'),
                        n('b')))),
            n('a')(
                n('b'),
                n('b'),
                n('b')(
                    n('a'),
                    n('a'),
                    n('b')(
                        n('a')(
                            n('b')),
                        n('a')),
                    n('b'),
                    n('a')),
                n('b')))};
    nary_tree<char> result;

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
};

void PatternTest2::test1() {
    pattern p {
        star()(
            one('b'),
            one('b'),
            one('b')(
                one('a'),
                one('a'),
                one('b')))};
    QVERIFY(p.search(this->tree));
    p.assign_result(result);
    QCOMPARE(
        result,
        n('a')(
            n('a')(
                n('a')(
                    n('b'),
                    n('b')),
                n('b'),
                n('b')(
                    n('a'),
                    n('a'),
                    n('b'))),
            n('a')(
                n('b'),
                n('b'),
                n('b')(
                    n('a')),
                n('b'))));
}

void PatternTest2::test2() {
    pattern p {star()};
    QVERIFY(p.search(this->tree));
    p.assign_result(result);
    QCOMPARE(result, n('a'));
}

void PatternTest2::test3() {
    pattern p {star()()}; // Has an additional ()
    QVERIFY(p.search(this->tree));
    p.assign_result(result);
    QCOMPARE(result, n('a'));
}

void PatternTest2::test4() {
    pattern p {
        star<quantifier::RELUCTANT>()(
            one('a')(
                one('b')(
                    one('a')(
                        one('b')))))};
    QVERIFY(p.search(this->tree));
    p.assign_result(result);
    QCOMPARE(
        result,
        n('a')(
            n('a')(
                n('a')(
                    n('b')(
                        n('a')(
                            n('b')))))));
}

void PatternTest2::test5() {
    pattern p {
        star()(
            cpt(
                one()(
                    one('b'),
                    one('b'),
                    one('b'),
                    one('b'))))};
    QVERIFY(p.search(this->tree));
    p.assign_mark(const_index<1>(), result);
    QCOMPARE(
        result,
        n('a')(
            n('b'),
            n('b'),
            n('b'),
            n('b')));
}

void PatternTest2::test6() {
    pattern p {
        star<quantifier::POSSESSIVE>('a')(
            cpt(
                opt('a')(
                    one('b')(
                        one('a'),
                        one('a')))))};
    QVERIFY(p.search(this->tree));
    p.assign_result(this->result);
    QCOMPARE(
        result,
        n('a')(
            n('a')(
                n('a')(
                    n('b')(
                        n('a'),
                        n('a')),
                    n('a'))),
            n('a')));
}

void PatternTest2::test7() {
}

void PatternTest2::test8() {
}

void PatternTest2::test9() {
}

void PatternTest2::test10() {
}

QTEST_MAIN(PatternTest2);
#include "PatternTest2.moc"
