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
        policy::post_order,
        CustomAllocator<Foo>>
        tree {};

    QCOMPARE(CustomAllocator<binary_node<Foo>>::allocated.size(), 0);
    QCOMPARE(CustomAllocator<binary_node<Foo>>::total_allocated, 0);
    QCOMPARE(CustomAllocator<binary_node<Foo>>::total_deallocated, 0);

    // Assigning 5 nodes
    tree
        = n(1, 2)(
            n(2, 3)(
                n(5, 6),
                n(6, 7)),
            n(3, 4));

    QCOMPARE(CustomAllocator<binary_node<Foo>>::allocated.size(), 5);
    QCOMPARE(CustomAllocator<binary_node<Foo>>::total_allocated, 5);
    QCOMPARE(CustomAllocator<binary_node<Foo>>::total_deallocated, 0);

    // Replacing 1 node with 1 node
    tree.emplace_over(
        std::find(tree.begin(policy::pre_order()), tree.end(policy::pre_order()), Foo(3, 4)),
        6, 7);

    QCOMPARE(CustomAllocator<binary_node<Foo>>::allocated.size(), 5);
    QCOMPARE(CustomAllocator<binary_node<Foo>>::total_allocated, 6);
    QCOMPARE(CustomAllocator<binary_node<Foo>>::total_deallocated, 1);

    // Replacing 3 nodes with 5
    tree.insert_over(
        std::find(tree.begin(policy::in_order()), tree.end(policy::in_order()), Foo(2, 3)),
        n(Foo(7, 8))(
            n(),
            n(Foo(8, 9))(
                n(Foo(9, 10)),
                n(Foo(10, 11))(
                    n(Foo(11, 12))))));

    QCOMPARE(CustomAllocator<binary_node<Foo>>::allocated.size(), 7);
    QCOMPARE(CustomAllocator<binary_node<Foo>>::total_allocated, 11);
    QCOMPARE(CustomAllocator<binary_node<Foo>>::total_deallocated, 4);

    // Adding 3 nodes
    tree.emplace_child_front(
        std::find(tree.begin(), tree.end(), Foo(7, 8)),
        n(-1, -2)(
            n(-3, -4),
            n(-5, -6)));

    QCOMPARE(CustomAllocator<binary_node<Foo>>::allocated.size(), 10);
    QCOMPARE(CustomAllocator<binary_node<Foo>>::total_allocated, 14);
    QCOMPARE(CustomAllocator<binary_node<Foo>>::total_deallocated, 4);

    { // Playing in a sub-scope
        nary_tree<
            Foo,
            policy::breadth_first,
            CustomAllocator<Foo>>
            copyed(tree);

        QCOMPARE(CustomAllocator<nary_node<Foo>>::allocated.size(), 10);
        QCOMPARE(CustomAllocator<nary_node<Foo>>::total_allocated, 10);
        QCOMPARE(CustomAllocator<nary_node<Foo>>::total_deallocated, 0);

        // Replacing all 10 nodes with 2
        copyed = n(Foo(0, 1))(n(Foo(1, 2)));

        QCOMPARE(CustomAllocator<nary_node<Foo>>::allocated.size(), 2);
        QCOMPARE(CustomAllocator<nary_node<Foo>>::total_allocated, 12);
        QCOMPARE(CustomAllocator<nary_node<Foo>>::total_deallocated, 10);

        // Move constructing a tree does not ask the allocator
        nary_tree<Foo, policy::pre_order, CustomAllocator<Foo>> moved(std::move(copyed));

        QCOMPARE(CustomAllocator<nary_node<Foo>>::allocated.size(), 2);
        QCOMPARE(CustomAllocator<nary_node<Foo>>::total_allocated, 12);
        QCOMPARE(CustomAllocator<nary_node<Foo>>::total_deallocated, 10);

        // Move assigning a tree does not ask the allocator
        nary_tree<Foo, policy::pre_order, CustomAllocator<Foo>> moveds = std::move(copyed);

        QCOMPARE(CustomAllocator<nary_node<Foo>>::allocated.size(), 2);
        QCOMPARE(CustomAllocator<nary_node<Foo>>::total_allocated, 12);
        QCOMPARE(CustomAllocator<nary_node<Foo>>::total_deallocated, 10);

    } // Implicit clear here

    QCOMPARE(CustomAllocator<nary_node<Foo>>::allocated.size(), 0);
    QCOMPARE(CustomAllocator<nary_node<Foo>>::total_allocated, 12);
    QCOMPARE(CustomAllocator<nary_node<Foo>>::total_deallocated, 12);

    // Adding 3 nodes
    tree.insert_child_back(
        tree.begin(policy::post_order()),
        Foo(100, 101));
    tree.insert_child_front(
        tree.begin(policy::post_order()),
        n(Foo(102, 103))(
            n(Foo(104, 105))));

    QCOMPARE(CustomAllocator<binary_node<Foo>>::allocated.size(), 13);
    QCOMPARE(CustomAllocator<binary_node<Foo>>::total_allocated, 17);
    QCOMPARE(CustomAllocator<binary_node<Foo>>::total_deallocated, 4);

    // Erasing 2 nodes, adding 5 nodes
    tree.erase(std::find(tree.begin(), tree.end(), Foo(102, 103)));
    tree.emplace_child_back(
        tree.begin(policy::post_order()),
        -1, -2);
    tree.emplace_child_front(
        tree.begin(policy::post_order()),
        n(-2, 3)(
            n(-3, -4)(
                n(),
                n(-5, 6)),
            n(-4, -5)));

    QCOMPARE(CustomAllocator<binary_node<Foo>>::allocated.size(), 16);
    QCOMPARE(CustomAllocator<binary_node<Foo>>::total_allocated, 22);
    QCOMPARE(CustomAllocator<binary_node<Foo>>::total_deallocated, 6);

    // Creating a copy tree
    binary_tree<Foo, policy::breadth_first, CustomAllocator<Foo>> tree2;
    tree2.insert_over(tree2.begin(), tree);

    QCOMPARE(CustomAllocator<binary_node<Foo>>::allocated.size(), 32);
    QCOMPARE(CustomAllocator<binary_node<Foo>>::total_allocated, 38);
    QCOMPARE(CustomAllocator<binary_node<Foo>>::total_deallocated, 6);

    // Moving the tree back
    tree.insert_child_back(tree.begin(policy::post_order()), std::move(tree2));

    QCOMPARE(CustomAllocator<binary_node<Foo>>::allocated.size(), 32);
    QCOMPARE(CustomAllocator<binary_node<Foo>>::total_allocated, 38);
    QCOMPARE(CustomAllocator<binary_node<Foo>>::total_deallocated, 6);

    // Erasing all the ndes
    tree.clear();

    QCOMPARE(CustomAllocator<binary_node<Foo>>::allocated.size(), 0);
    QCOMPARE(CustomAllocator<binary_node<Foo>>::total_allocated, 38);
    QCOMPARE(CustomAllocator<binary_node<Foo>>::total_deallocated, 38);
}

void AllocationTest::test2() {
    // Empty bin creation
    binary_tree<Bar, policy::in_order, CustomAllocator<Bar>> bin;
    QCOMPARE(CustomAllocator<binary_node<Bar>>::allocated.size(), 0);
    QCOMPARE(CustomAllocator<binary_node<Bar>>::total_allocated, 0);
    QCOMPARE(CustomAllocator<binary_node<Bar>>::total_deallocated, 0);

    // Empty bin2 creation
    binary_tree<Bar, policy::post_order, CustomAllocator<Bar>> bin2;
    QCOMPARE(CustomAllocator<binary_node<Bar>>::allocated.size(), 0);
    QCOMPARE(CustomAllocator<binary_node<Bar>>::total_allocated, 0);
    QCOMPARE(CustomAllocator<binary_node<Bar>>::total_deallocated, 0);

    // Adding 5 nodes to bin
    bin.emplace_over(
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
    QCOMPARE(CustomAllocator<binary_node<Bar>>::allocated.size(), 5);
    QCOMPARE(CustomAllocator<binary_node<Bar>>::total_allocated, 5);
    QCOMPARE(CustomAllocator<binary_node<Bar>>::total_deallocated, 0);

    // Deleting 2 nodes subtree from bin2
    bin2.erase(std::find(bin2.begin(policy::post_order()), bin2.end(policy::post_order()), Bar(2, 3)));
    QCOMPARE(bin2.size(), 3);
    QCOMPARE(CustomAllocator<binary_node<Bar>>::allocated.size(), 3);
    QCOMPARE(CustomAllocator<binary_node<Bar>>::total_allocated, 5);
    QCOMPARE(CustomAllocator<binary_node<Bar>>::total_deallocated, 2);

    // Replacing 1 node in bin2
    bin2.emplace_over(std::find(bin2.begin(), bin2.end(), Bar(5, 6)), 100, 101);
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

    // Adding 1 node to bin
    bin.insert_over(bin.begin(), Bar(200, 201));
    QCOMPARE(CustomAllocator<binary_node<Bar>>::allocated.size(), 1);
    QCOMPARE(CustomAllocator<binary_node<Bar>>::total_allocated, 7);
    QCOMPARE(CustomAllocator<binary_node<Bar>>::total_deallocated, 6);

    // Replacing 1 node with 7 nodes
    bin.insert_over(
        bin.begin(policy::pre_order()),
        binary_tree<Bar, policy::post_order, CustomAllocator<Bar>>(
            n(Bar(200, 201))(
                n(Bar(201, 202))(
                    n(Bar(202, 203)),
                    n(Bar(203, 204))),
                n(Bar(300, 301))(
                    n(),
                    n(Bar(301, 302))(
                        n(),
                        n(Bar(302, 303)))))));
    QCOMPARE(bin.size(), 7);
    QCOMPARE(CustomAllocator<binary_node<Bar>>::allocated.size(), 7);
    QCOMPARE(CustomAllocator<binary_node<Bar>>::total_allocated, 14);
    QCOMPARE(CustomAllocator<binary_node<Bar>>::total_deallocated, 7);

    {
        // Moving bin to bin3
        binary_tree<Bar, policy::pre_order, CustomAllocator<Bar>> bin3(std::move(bin));
        QCOMPARE(CustomAllocator<binary_node<Bar>>::allocated.size(), 7);
        QCOMPARE(CustomAllocator<binary_node<Bar>>::total_allocated, 14);
        QCOMPARE(CustomAllocator<binary_node<Bar>>::total_deallocated, 7);

        // Situation unchanged (bin2 is empty)
        bin3.insert_child_front(
            std::find(bin3.begin(), bin3.end(), Bar(300, 301)),
            bin2);
        QCOMPARE(CustomAllocator<binary_node<Bar>>::allocated.size(), 7);
        QCOMPARE(CustomAllocator<binary_node<Bar>>::total_allocated, 14);
        QCOMPARE(CustomAllocator<binary_node<Bar>>::total_deallocated, 7);

        // Adding 5 nodes to bin3
        bin3.insert_child_back(
            std::find(bin3.begin(), bin3.end(), Bar(300, 301)),
            binary_tree<Bar, policy::breadth_first, CustomAllocator<Bar>>(
                n(-1, -2)(
                    n(-2, -3)(
                        n(-4, -5)),
                    n(-3, -4)(
                        n(),
                        n(-5, -6)))));
        QCOMPARE(CustomAllocator<binary_node<Bar>>::allocated.size(), 12);
        QCOMPARE(CustomAllocator<binary_node<Bar>>::total_allocated, 19);
        QCOMPARE(CustomAllocator<binary_node<Bar>>::total_deallocated, 7);
    }

    // Erasing bin3
    QCOMPARE(CustomAllocator<binary_node<Bar>>::allocated.size(), 0);
    QCOMPARE(CustomAllocator<binary_node<Bar>>::total_allocated, 19);
    QCOMPARE(CustomAllocator<binary_node<Bar>>::total_deallocated, 19);
}

// This is a meta test ;)
// Distrust who trusts its code
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
