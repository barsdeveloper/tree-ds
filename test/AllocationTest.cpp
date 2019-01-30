#include <QtTest/QtTest>

#include <TreeDS/tree>

#include "Types.hpp"

using namespace std;
using namespace md;

class AllocationTest : public QObject {

    Q_OBJECT

    private slots:
    void test1();
    void test2();
    void testAllocatorUsedForTests();
};

void AllocationTest::test1() {
    binary_tree<
        Foo,
        post_order,
        CustomAllocator<Foo>>
        tree {};

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
            breadth_first,
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

void AllocationTest::test2() {
    // Empty bin creation
    binary_tree<Bar, in_order, CustomAllocator<Bar>> bin;
    QCOMPARE(CustomAllocator<binary_node<Bar>>::allocated.size(), 0);
    QCOMPARE(CustomAllocator<binary_node<Bar>>::total_allocated, 0);
    QCOMPARE(CustomAllocator<binary_node<Bar>>::total_deallocated, 0);

    // Empty bin2 creation
    binary_tree<Bar, post_order, CustomAllocator<Bar>> bin2;
    QCOMPARE(CustomAllocator<binary_node<Bar>>::allocated.size(), 0);
    QCOMPARE(CustomAllocator<binary_node<Bar>>::total_allocated, 0);
    QCOMPARE(CustomAllocator<binary_node<Bar>>::total_deallocated, 0);

    // Adding 5-sized subtree to bin
    bin.emplace(
        bin.begin(),
        n(1, 2)(
            n(2, 3)(
                n(4, 5)(
                    n())),
            n(3, 4)(
                n(),
                n(5, 6)(
                    n()))));
    QCOMPARE(bin.size(), 5);
    QCOMPARE(CustomAllocator<binary_node<Bar>>::allocated.size(), 5);
    QCOMPARE(CustomAllocator<binary_node<Bar>>::total_allocated, 5);
    QCOMPARE(CustomAllocator<binary_node<Bar>>::total_deallocated, 0);

    // Swapping bin and bin2
    swap(bin, bin2);
    QCOMPARE(bin2.size(), 5);
    QVERIFY(bin.empty());
    QCOMPARE(CustomAllocator<binary_node<Bar>>::allocated.size(), 5);
    QCOMPARE(CustomAllocator<binary_node<Bar>>::total_allocated, 5);
    QCOMPARE(CustomAllocator<binary_node<Bar>>::total_deallocated, 0);

    // Deleting 2-sized subtree from bin2
    bin2.erase(std::find(bin2.begin<post_order>(), bin2.end<post_order>(), Bar(2, 3)));
    QCOMPARE(bin2.size(), 3);
    QCOMPARE(CustomAllocator<binary_node<Bar>>::allocated.size(), 3);
    QCOMPARE(CustomAllocator<binary_node<Bar>>::total_allocated, 5);
    QCOMPARE(CustomAllocator<binary_node<Bar>>::total_deallocated, 2);

    // Replacing one now in bin2
    bin2.emplace(std::find(bin2.begin(), bin2.end(), Bar(5, 6)), 100, 101);
    QCOMPARE(bin2.size(), 3);
    QCOMPARE(CustomAllocator<binary_node<Bar>>::allocated.size(), 3);
    QCOMPARE(CustomAllocator<binary_node<Bar>>::total_allocated, 6);
    QCOMPARE(CustomAllocator<binary_node<Bar>>::total_deallocated, 3);

    // Moving bin2 to bin
    bin = std::move(bin2);
    QCOMPARE(bin.size(), 3);
    QCOMPARE(bin2.size(), 0);
    QCOMPARE(CustomAllocator<binary_node<Bar>>::allocated.size(), 3);
    QCOMPARE(CustomAllocator<binary_node<Bar>>::total_allocated, 6);
    QCOMPARE(CustomAllocator<binary_node<Bar>>::total_deallocated, 3);

    // Assigning empty bin2 to bin
    bin = bin2;
    QCOMPARE(bin.size(), 0);
    QCOMPARE(bin2.size(), 0);
    QCOMPARE(CustomAllocator<binary_node<Bar>>::allocated.size(), 0);
    QCOMPARE(CustomAllocator<binary_node<Bar>>::total_allocated, 6);
    QCOMPARE(CustomAllocator<binary_node<Bar>>::total_deallocated, 6);

    // Assigning empty bin to bin2
    bin2 = bin;
    QCOMPARE(CustomAllocator<binary_node<Bar>>::allocated.size(), 0);
    QCOMPARE(CustomAllocator<binary_node<Bar>>::total_allocated, 6);
    QCOMPARE(CustomAllocator<binary_node<Bar>>::total_deallocated, 6);

    // Moving empty bin to bin2
    bin2 = std::move(bin);
    QCOMPARE(CustomAllocator<binary_node<Bar>>::allocated.size(), 0);
    QCOMPARE(CustomAllocator<binary_node<Bar>>::total_allocated, 6);
    QCOMPARE(CustomAllocator<binary_node<Bar>>::total_deallocated, 6);

    // Assing 7-sized tree to bin
    bin.insert(bin.begin(), Bar(200, 201));
    QCOMPARE(CustomAllocator<binary_node<Bar>>::allocated.size(), 1);
    QCOMPARE(CustomAllocator<binary_node<Bar>>::total_allocated, 7);
    QCOMPARE(CustomAllocator<binary_node<Bar>>::total_deallocated, 6);

    bin.insert(
        bin.begin(),
        n(Bar(200, 201))(
            n(Bar(201, 202))(
                n(Bar(202, 203)),
                n(Bar(203, 204))),
            n(Bar(300, 301))(
                n(),
                n(Bar(301, 302))(
                    n(),
                    n(Bar(302, 303))))));
    QCOMPARE(bin.size(), 7);
    QCOMPARE(CustomAllocator<binary_node<Bar>>::allocated.size(), 7);
    QCOMPARE(CustomAllocator<binary_node<Bar>>::total_allocated, 14);
    QCOMPARE(CustomAllocator<binary_node<Bar>>::total_deallocated, 7);

    {
        binary_tree<Bar, pre_order, CustomAllocator<Bar>> bin3(std::move(bin));
        QCOMPARE(CustomAllocator<binary_node<Bar>>::allocated.size(), 7);
        QCOMPARE(CustomAllocator<binary_node<Bar>>::total_allocated, 14);
        QCOMPARE(CustomAllocator<binary_node<Bar>>::total_deallocated, 7);
    }
    QCOMPARE(CustomAllocator<binary_node<Bar>>::allocated.size(), 0);
    QCOMPARE(CustomAllocator<binary_node<Bar>>::total_allocated, 14);
    QCOMPARE(CustomAllocator<binary_node<Bar>>::total_deallocated, 14);
}

void AllocationTest::testAllocatorUsedForTests() {
    using allocator_t = CustomAllocator<binary_node<float>>;

    {
        allocator_t allocator;

        QCOMPARE(allocator_t::allocated.size(), 0);
        QCOMPARE(allocator_t::total_allocated, 0);
        QCOMPARE(allocator_t::total_deallocated, 0);

        auto ptr  = allocate(allocator, 5.7);
        auto ptr2 = allocate(allocator, 7.8);

        QCOMPARE(allocator_t::allocated.size(), 2);
        QCOMPARE(allocator_t::total_allocated, 2);
        QCOMPARE(allocator_t::total_deallocated, 0);
    } // here allocated stuff will be deleted

    QCOMPARE(allocator_t::allocated.size(), 0);
    QCOMPARE(allocator_t::total_allocated, 2);
    QCOMPARE(allocator_t::total_deallocated, 2);
}

QTEST_MAIN(AllocationTest);
#include "AllocationTest.moc"
