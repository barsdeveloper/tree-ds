#include <QtTest/QtTest>

#include <TreeDS/tree>

#include "Types.hpp"

using namespace std;
using namespace ds;

class AllocationTest : public QObject {

    Q_OBJECT

    private slots:
    void first();
};

void AllocationTest::first() {
    binary_tree<
        Foo,
        post_order,
        CustomAllocator<Foo>>
        tree{};

    QCOMPARE(tree.get_allocator().allocated.size(), 0);
    QCOMPARE(tree.get_allocator().total_allocated, 0);
    QCOMPARE(tree.get_allocator().total_deallocated, 0);

    // Assigning 5 nodes
    tree
        = n(1, 2)(
            n(2, 3)(
                n(5, 6),
                n(6, 7)),
            n(3, 4));

    QCOMPARE(tree.get_allocator().allocated.size(), 5);
    QCOMPARE(tree.get_allocator().total_allocated, 5);
    QCOMPARE(tree.get_allocator().total_deallocated, 0);

    // Replacing 1 node with 1 node
    tree.emplace(
        std::find(tree.begin<pre_order>(), tree.end<pre_order>(), Foo(3, 4)),
        6, 7);

    QCOMPARE(tree.get_allocator().allocated.size(), 5);
    QCOMPARE(tree.get_allocator().total_allocated, 6);
    QCOMPARE(tree.get_allocator().total_deallocated, 1);

    // Replacing 3 nodes with 5
    tree.insert(
        std::find(tree.begin<in_order>(), tree.end<in_order>(), Foo(2, 3)),
        n(Foo(7, 8))(
            n(),
            n(Foo(8, 9))(
                n(Foo(9, 10)),
                n(Foo(10, 11))(
                    n(Foo(11, 12))))));

    QCOMPARE(tree.get_allocator().allocated.size(), 7);
    QCOMPARE(tree.get_allocator().total_allocated, 11);
    QCOMPARE(tree.get_allocator().total_deallocated, 4);

    { // Playing in a sub-scope
        nary_tree<
            Foo,
            breadth_first<
                nary_node<Foo>,
                CustomAllocator<nary_node<Foo>>>,
            CustomAllocator<Foo>>
            copyed(tree);

        QCOMPARE(CustomAllocator<nary_node<Foo>>::allocated.size(), 7);
        QCOMPARE(CustomAllocator<nary_node<Foo>>::total_allocated, 7);
        QCOMPARE(CustomAllocator<nary_node<Foo>>::total_deallocated, 0);

        // Replacing all 7 nodes with 2
        copyed = n(Foo(0, 1))(n(Foo(1, 2)));

        QCOMPARE(CustomAllocator<nary_node<Foo>>::allocated.size(), 2);
        QCOMPARE(CustomAllocator<nary_node<Foo>>::total_allocated, 9);
        QCOMPARE(CustomAllocator<nary_node<Foo>>::total_deallocated, 7);

        // Move constructing a tree does not ask the allocator
        nary_tree<Foo, pre_order, CustomAllocator<Foo>> moved(std::move(copyed));

        QCOMPARE(CustomAllocator<nary_node<Foo>>::allocated.size(), 2);
        QCOMPARE(CustomAllocator<nary_node<Foo>>::total_allocated, 9);
        QCOMPARE(CustomAllocator<nary_node<Foo>>::total_deallocated, 7);

        // Move assigning a tree does not ask the allocator
        nary_tree<Foo, pre_order, CustomAllocator<Foo>> moveds = std::move(copyed);

        QCOMPARE(CustomAllocator<nary_node<Foo>>::allocated.size(), 2);
        QCOMPARE(CustomAllocator<nary_node<Foo>>::total_allocated, 9);
        QCOMPARE(CustomAllocator<nary_node<Foo>>::total_deallocated, 7);

    } // Implicit clear here

    QCOMPARE(CustomAllocator<nary_node<Foo>>::allocated.size(), 0);
    QCOMPARE(CustomAllocator<nary_node<Foo>>::total_allocated, 9);
    QCOMPARE(CustomAllocator<nary_node<Foo>>::total_deallocated, 9);

    // Now the allocator has no object allocated and counters are equal
    tree.clear();

    QCOMPARE(tree.get_allocator().allocated.size(), 0);
    QCOMPARE(tree.get_allocator().total_allocated, 11);
    QCOMPARE(tree.get_allocator().total_deallocated, 11);
}

QTEST_MAIN(AllocationTest);
#include "AllocationTest.moc"
