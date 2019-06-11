#include <QtTest/QtTest>

#include <TreeDS/node/multiple_node_pointer.hpp>
#include <TreeDS/tree>

#include "Types.hpp"

using namespace std;
using namespace md;

class MultipleNodePointerTest : public QObject {

    Q_OBJECT

    private slots:
    void binary();
    void nary();
};

void MultipleNodePointerTest::binary() {
    binary_node<int> bin1 {
        n(1)(
            n(2)(
                n(4),
                n(5)),
            n(3)(
                n(),
                n(6))),
        allocator<binary_node<int>>()};
    binary_node<int> bin2 {bin1, allocator<binary_node<int>>()};
    binary_node<int> bin3 {bin1, allocator<binary_node<int>>()};
    multiple_node_pointer pointer(&bin1, &bin2, &bin3);

    QCOMPARE(
        make_tuple(1, 1, 1),
        apply(
            [](auto&&... pointer) { return make_tuple(pointer->get_value()...); },
            pointer.get()));

    pointer = pointer->get_first_child();
    QCOMPARE(
        make_tuple(2, 2, 2),
        apply(
            [](auto&&... pointer) { return make_tuple(pointer->get_value()...); },
            pointer.get()));

    pointer = pointer->get_first_child();
    QCOMPARE(
        make_tuple(4, 4, 4),
        apply(
            [](auto&&... pointer) { return make_tuple(pointer->get_value()...); },
            pointer.get()));

    pointer = pointer->get_next_sibling();
    QCOMPARE(
        make_tuple(5, 5, 5),
        apply(
            [](auto&&... pointer) { return make_tuple(pointer->get_value()...); },
            pointer.get()));

    pointer = pointer->get_prev_sibling();
    QCOMPARE(
        make_tuple(4, 4, 4),
        apply(
            [](auto&&... pointer) { return make_tuple(pointer->get_value()...); },
            pointer.get()));

    pointer = pointer->get_parent()->get_next_sibling()->get_right_child();
    QCOMPARE(
        make_tuple(6, 6, 6),
        apply(
            [](auto&&... pointer) { return make_tuple(pointer->get_value()...); },
            pointer.get()));

    pointer = pointer->get_parent()->get_parent()->get_left_child();
    QCOMPARE(
        make_tuple(2, 2, 2),
        apply(
            [](auto&&... pointer) { return make_tuple(pointer->get_value()...); },
            pointer.get()));
}

void MultipleNodePointerTest::nary() {
    nary_node<int> nar1 {
        n(-1)(
            n(-2)(
                n(-3)(
                    n(-4),
                    n(-5)(
                        n(-7),
                        n(-8)),
                    n(-6)(
                        n(-9)))))};
    nary_node<int> nar2 {
        n(-1)(
            n(-2)(
                n(-3)(
                    n(-4),
                    n(-5)(
                        n(-7),
                        n(-8)),
                    n(-6)))),
        allocator<nary_node<int>>()};
    nary_node<int> nar3 {nar1, allocator<nary_node<int>>()};
    nary_node<int> nar4 {nar1, allocator<nary_node<int>>()};
    multiple_node_pointer pointer(&nar1, &nar2, &nar3, &nar4);

    QCOMPARE(
        make_tuple(-1, -1, -1, -1),
        apply(
            [](auto&&... pointer) { return make_tuple(pointer->get_value()...); },
            pointer.get()));

    pointer = pointer->get_child(0);
    QCOMPARE(
        make_tuple(-2, -2, -2, -2),
        apply(
            [](auto&&... pointer) { return make_tuple(pointer->get_value()...); },
            pointer.get()));

    pointer = pointer->get_first_child()->get_child(1);
    QCOMPARE(
        make_tuple(-5, -5, -5, -5),
        apply(
            [](auto&&... pointer) { return make_tuple(pointer->get_value()...); },
            pointer.get()));

    pointer = pointer->get_next_sibling();
    QCOMPARE(
        make_tuple(-6, -6, -6, -6),
        apply(
            [](auto&&... pointer) { return make_tuple(pointer->get_value()...); },
            pointer.get()));

    /* pointer = pointer->get_last_child();
    QCOMPARE(
        make_tuple(-9, nullptr, -6, -6),
        apply(
            [](auto&&... pointer) { return make_tuple((pointer != nullptr ? pointer->get_value() : nullptr)...); },
            pointer.get()));*/
}

QTEST_MAIN(MultipleNodePointerTest);
#include "MultipleNodePointerTest.moc"
