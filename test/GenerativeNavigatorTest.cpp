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
    void binary();
    void nary();
};

void GenerativeNavigatorTest::binary() {
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
}

void GenerativeNavigatorTest::nary() {
}

QTEST_MAIN(GenerativeNavigatorTest);
#include "GenerativeNavigatorTest.moc"
