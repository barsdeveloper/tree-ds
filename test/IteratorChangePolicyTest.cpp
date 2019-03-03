#include <QtTest/QtTest>
#include <iterator> // std::advance()

#include <TreeDS/tree>

#include "Types.hpp"

using namespace std;
using namespace md;

class IteratorChangePolicyTest : public QObject {

    Q_OBJECT

    binary_tree<int> binary
        = n(1)(
            n(),
            n(2)(
                n(3)(
                    n(5)(
                        n(7),
                        n(8)(
                            n(11)(
                                n(15)(
                                    n(21)(
                                        n(25)(
                                            n(),
                                            n(29))),
                                    n(22)),
                                n(16)),
                            n(12)(
                                n(17),
                                n(18))))),
                n(4)(
                    n(6)(
                        n(9),
                        n(10)(
                            n(13)(
                                n(19)(
                                    n(23)(
                                        n(26)(
                                            n(30)),
                                        n(27)(
                                            n(),
                                            n(31))))),
                            n(14)(
                                n(20)(
                                    n(),
                                    n(24)(
                                        n(28)(
                                            n(),
                                            n(32))))))))));

    private slots:
    void test1();
};

void IteratorChangePolicyTest::test1() {
    // start in_order
    auto it_a = this->binary.begin(policy::in_order());
    QCOMPARE(*it_a, 1);
    QCOMPARE(*++it_a, 7);
    QCOMPARE(*++it_a, 5);

    // continue forward breadth_first
    auto it_b = it_a.other_policy(policy::breadth_first());
    QCOMPARE(*it_b, 5);
    QCOMPARE(*++it_b, 6);
    QCOMPARE(*++it_b, 7);
    QCOMPARE(*++it_b, 8);
    QCOMPARE(*++it_b, 9);
    QCOMPARE(*++it_b, 10);
    QCOMPARE(*++it_b, 11);

    auto it_c = it_b.other_policy(policy::pre_order());
    QCOMPARE(*it_c, 11);
    QCOMPARE(*++it_c, 15);
    QCOMPARE(*++it_c, 21);
    QCOMPARE(*++it_c, 25);
    QCOMPARE(*++it_c, 29);

    auto it_d = it_c.other_policy(policy::post_order());
    QCOMPARE(*it_d, 29);
    QCOMPARE(*++it_d, 25);
    QCOMPARE(*++it_d, 21);
    QCOMPARE(*++it_d, 22);
    QCOMPARE(*++it_d, 15);
    QCOMPARE(*++it_d, 16);

    auto it_e = it_d.other_policy(policy::pre_order());
    QCOMPARE(*it_e, 16);
    QCOMPARE(*++it_e, 12);
    QCOMPARE(*++it_e, 17);
    QCOMPARE(*++it_e, 18);
    QCOMPARE(*++it_e, 4);
    QCOMPARE(*++it_e, 6);
    QCOMPARE(*++it_e, 9);
    QCOMPARE(*++it_e, 10);
    QCOMPARE(*++it_e, 13);

    auto it_f = it_e.other_policy(policy::breadth_first());
    QCOMPARE(*it_f, 13);
    QCOMPARE(*++it_f, 14);
    QCOMPARE(*++it_f, 15);
    std::advance(it_f, -2);
    QCOMPARE(*--it_f, 12);
    QCOMPARE(*--it_f, 11);
    QCOMPARE(*--it_f, 10);
    QCOMPARE(*--it_f, 9);
    QCOMPARE(*--it_f, 8);
    QCOMPARE(*--it_f, 7);

    auto it_g = it_f.other_policy(policy::children());
    QCOMPARE(*it_g, 7);
    QCOMPARE(*++it_g, 8);
}

QTEST_MAIN(IteratorChangePolicyTest);
#include "IteratorChangePolicyTest.moc"
