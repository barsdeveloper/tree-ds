#include <QtTest/QtTest>
#include <memory>

#include <TreeDS/node/multiple_node_pointer.hpp>
#include <TreeDS/node/navigator/generative_navigator.hpp>
#include <TreeDS/tree>

#include "Types.hpp"

using namespace std;
using namespace md;

class GenerativeNavigatorTest : public QObject {

    Q_OBJECT

    private slots:
    void binary1();
    void binary2();
    void nary1();
};

void GenerativeNavigatorTest::binary1() {
    binary_tree<int> target {
        n(1)(
            n(2)(
                n(4),
                n(5)(
                    n(),
                    n(6))),
            n(3))};
    binary_tree<int> generated {n(1)};
    multiple_node_pointer ptrs(target.get_root().get_raw_node(), generated.get_root().get_raw_node());
    std::allocator<binary_node<int>> alloc;
    generative_navigator nav(
        alloc,
        ptrs,
        [](auto&&) {
            return true;
        });

    ptrs = nav.get_left_child(ptrs); // 2
    generated.update_size_arity();
    QCOMPARE(generated, n(1)(n(2)));

    ptrs = nav.get_next_sibling(ptrs); // 3
    generated.update_size_arity();
    QCOMPARE(generated, n(1)(n(2), n(3)));

    ptrs = nav.get_parent(ptrs); // 1
    generated.update_size_arity();
    QCOMPARE(generated, n(1)(n(2), n(3)));

    ptrs = nav.get_first_child(ptrs); // 2
    generated.update_size_arity();
    QCOMPARE(generated, n(1)(n(2), n(3)));

    ptrs = nav.get_last_child(ptrs); // 5
    generated.update_size_arity();
    QCOMPARE(
        generated,
        n(1)(
            n(2)(
                n(),
                n(5)),
            n(3)));

    ptrs = nav.get_first_child(ptrs); // 6
    generated.update_size_arity();
    QCOMPARE(
        generated,
        n(1)(
            n(2)(
                n(),
                n(5)(
                    n(),
                    n(6))),
            n(3)));

    ptrs = nav.get_parent(nav.get_parent(ptrs)); // 2
    generated.erase(
        std::find(
            generated.begin(policy::post_order()),
            generated.end(policy::post_order()),
            5));
    QCOMPARE(generated, n(1)(n(2), n(3)));

    ptrs = nav.get_right_child(ptrs); // 5
    generated.update_size_arity();
    QCOMPARE(
        generated,
        n(1)(
            n(2)(
                n(),
                n(5)),
            n(3)));
}

void GenerativeNavigatorTest::binary2() {
    binary_tree<int> target {
        n(-1)(
            n(-2)(
                n(-4)),
            n(3)(
                n(5)(
                    n(7),
                    n(8)(
                        n(9)(
                            n(),
                            n(11)),
                        n(-10)(
                            n(-12)))),
                n(6)))};
    binary_tree<int> generated {n(3)};
    multiple_node_pointer ptrs(target.get_root().go_last_child().get_raw_node(), generated.get_root().get_raw_node());
    std::allocator<binary_node<int>> alloc;
    generative_navigator nav(
        alloc,
        ptrs,
        [](auto node) {
            return node->get_value() >= 0;
        });

    ptrs = nav.get_right_child(ptrs); // 6
    generated.update_size_arity();
    QCOMPARE(generated, n(3)(n(), n(6)));

    ptrs = nav.get_prev_sibling(ptrs); // 5
    generated.update_size_arity();
    QCOMPARE(generated, n(3)(n(5), n(6)));

    ptrs = nav.get_last_child(ptrs); // 8
    generated.update_size_arity();
    QCOMPARE(
        generated,
        n(3)(
            n(5)(
                n(),
                n(8)),
            n(6)));

    ptrs = nav.get_last_child(ptrs); // 9
    generated.update_size_arity();
    QCOMPARE(
        generated,
        n(3)(
            n(5)(
                n(),
                n(8)(
                    n(9))),
            n(6)));

    auto other_ptr = nav.get_next_sibling(ptrs); // null
    generated.update_size_arity();
    QVERIFY(!other_ptr);
    QCOMPARE(
        generated,
        n(3)(
            n(5)(
                n(),
                n(8)(
                    n(9))),
            n(6)));
}

void GenerativeNavigatorTest::nary1() {
}

QTEST_MAIN(GenerativeNavigatorTest);
#include "GenerativeNavigatorTest.moc"
