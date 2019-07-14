#include <QtTest/QtTest>
#include <stdexcept>
#include <string>

#include <TreeDS/match>
#include <TreeDS/tree>

using namespace md;
using namespace std;

struct A {
    A() {
    }
    A(const A&) {
    }
    virtual ~A();
    virtual bool operator==(const A& other) const {
        return typeid(*this) == typeid(other);
    }
};
A::~A() {};
struct B : A {
    B() {
    }
    B(const B&) {
    }
    virtual ~B();
};
B::~B() {};
struct C : A {
    C() {
    }
    C(const C&) {
    }
    virtual ~C();
};
C::~C() {};
;
struct D : A {
    D() {
    }
    D(const D&) {
    }
    virtual ~D();
};
D::~D() {};

class PatternTypesTest : public QObject {

    Q_OBJECT

    B b1 {};
    B b2 {};
    C c1 {};
    C c2 {};
    D d1 {};
    D d2 {};

    constexpr static auto getter = [](const A* object) { return std::type_index(typeid(*object)); };
    nary_tree<A*> tree {
        n(&b1)(
            n(&b2),
            n (&c1)(
                n (&b1)(
                    n (&d1)(
                        n (&d1)(
                            n(&d1),
                            n(&d1)),
                        n(&d1)))),
            n (&d1)(
                n (&d1)(
                    n(&d1),
                    n(&d1)),
                n(&d1)),
            n (&b2)(
                n(&b1),
                n (&b2)(
                    n(&b1))))};
    nary_tree<A*> result;

    private slots:
    void test1();
};

void PatternTypesTest::test1() {
    pattern p {
        star<quantifier::RELUCTANT>()(
            one(having(type_index(typeid(D)), getter)))};
    QVERIFY(p.search(tree));
    p.assign_result(result);
    QCOMPARE(
        result,
        n(&b1)(
            n(&c1)(
                n(&b1)(
                    n(&d1)))));
}

QTEST_MAIN(PatternTypesTest);
#include "PatternTypesTest.moc"
