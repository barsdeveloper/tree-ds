#include <QtTest/QtTest>
#include <limits> // std::numeric_limits

#include <TreeDS/tree>

#include "Types.hpp"

using namespace std;
using namespace md;

class PreliminaryTest : public QObject {

    Q_OBJECT

    private slots:
    void emptyBinary();
    void emptyAllocatorBinary();
    void typeRequirements();
};

void PreliminaryTest::emptyBinary() {
    binary_tree<Foo> t;
    const binary_tree<Foo>& cRef = t;

    // The trees is empty
    QCOMPARE(t.size(), 0);
    QVERIFY(t.empty());

    // Iterator point to the end
    QVERIFY(t.begin() == t.end());
    QVERIFY(t.cbegin() == t.cend());
    QVERIFY(t.rbegin() == t.rend());
    QVERIFY(t.crbegin() == t.crend());

    // Iterator to const_iterator equality compatibility
    QVERIFY(t.cbegin() == t.begin());
    QVERIFY(t.cbegin() == t.end());
    QVERIFY(t.crbegin() == t.rend());
    QVERIFY(t.crbegin() == t.rbegin());
    QVERIFY(cRef.begin() == t.begin());

    // construct an element
    t.emplace(t.begin(), 67, 93);
    QCOMPARE(t, n(Foo(67, 93)));

    // The tree has 1 element
    QCOMPARE(t.size(), 1);
    QVERIFY(!t.empty());

    // All iterators begin from the unique element
    QCOMPARE(*t.begin(), Foo(67, 93));
    QCOMPARE(*t.cbegin(), Foo(67, 93));
    QCOMPARE(*t.rbegin(), Foo(67, 93));
    QCOMPARE(*t.crbegin(), Foo(67, 93));
    QCOMPARE(*cRef.begin(), Foo(67, 93));

    // Iterator no more point to the end
    QVERIFY(t.begin() != t.end());
    QVERIFY(t.cbegin() != t.cend());
    QVERIFY(t.rbegin() != t.rend());
    QVERIFY(t.crbegin() != t.crend());
    QVERIFY(cRef.begin() != cRef.end());

    // End is one after begin
    QVERIFY(++t.begin() == t.end());
    QVERIFY(++t.cbegin() == t.cend());
    QVERIFY(++t.rbegin() == t.rend());
    QVERIFY(++t.crbegin() == t.crend());
    QVERIFY(++cRef.begin() == cRef.end());
    QVERIFY(++cRef.rbegin() == cRef.rend());

    // Begin is one before end
    QVERIFY(t.begin() == --t.end());
    QVERIFY(t.cbegin() == --t.cend());
    QVERIFY(t.rbegin() == --t.rend());
    QVERIFY(t.crbegin() == --t.crend());
    QVERIFY(cRef.begin() == --cRef.end());
    QVERIFY(cRef.rbegin() == --cRef.rend());

    // Delete all elements
    t.clear();

    // The tree is again empty
    QCOMPARE(t.size(), 0);
    QVERIFY(t.empty());

    // Iterators point again at the end
    QVERIFY(t.begin() == t.end());
    QVERIFY(t.cbegin() == t.cend());
    QVERIFY(t.rbegin() == t.rend());
    QVERIFY(t.crbegin() == t.crend());

    // Create by insert
    t.insert(t.begin(), Foo(45, 900));
    QCOMPARE(*t.begin(), Foo(45, 900));
    QCOMPARE(t, n(Foo(45, 900)));

    // The tree has 1 element again
    QCOMPARE(t.size(), 1);
    QVERIFY(!t.empty());
    QCOMPARE(t.max_size(), std::numeric_limits<std::size_t>::max());
}

void PreliminaryTest::emptyAllocatorBinary() {
    CustomAllocator<std::string> allocator;
    binary_tree<
        std::string,
        breadth_first,
        CustomAllocator<std::string>>
        t(allocator);

    // The trees is empty
    QCOMPARE(t.size(), 0);
    QVERIFY(t.empty());

    // Iterator point to the end
    QVERIFY(t.begin() == t.end());
    QVERIFY(t.cbegin() == t.cend());
    QVERIFY(t.rbegin() == t.rend());
    QVERIFY(t.crbegin() == t.crend());

    // Iterator to const_iterator equality compatibility
    QVERIFY(t.cbegin() == t.begin());
    QVERIFY(t.cbegin() == t.end());
    QVERIFY(t.crbegin() == t.rend());
    QVERIFY(t.crbegin() == t.rbegin());

    // check allocator status
    QCOMPARE(CustomAllocator<binary_node<std::string>>::allocated.size(), 0);
    QCOMPARE(CustomAllocator<binary_node<std::string>>::total_allocated, 0);
    QCOMPARE(CustomAllocator<binary_node<std::string>>::total_deallocated, 0);

    // construct an element
    t.emplace(t.begin(), "hello");
    QCOMPARE(t, n(string("hello")));
    QCOMPARE(t, n("hello"));

    // check allocator status
    QCOMPARE(CustomAllocator<binary_node<std::string>>::allocated.size(), 1);
    QCOMPARE(CustomAllocator<binary_node<std::string>>::total_allocated, 1);
    QCOMPARE(CustomAllocator<binary_node<std::string>>::total_deallocated, 0);

    // The tree has 1 element
    QCOMPARE(t.size(), 1);
    QVERIFY(!t.empty());

    // All iterators begin from the unique element
    QCOMPARE(*t.begin(), "hello");
    QCOMPARE(*t.cbegin(), "hello");
    QCOMPARE(*t.rbegin(), "hello");
    QCOMPARE(*t.crbegin(), "hello");

    // Iterator no more point to the end
    QVERIFY(t.begin() != t.end());
    QVERIFY(t.cbegin() != t.cend());
    QVERIFY(t.rbegin() != t.rend());
    QVERIFY(t.crbegin() != t.crend());

    // End is one after begin
    QVERIFY(++t.begin() == t.end());
    QVERIFY(++t.cbegin() == t.cend());
    QVERIFY(++t.rbegin() == t.rend());
    QVERIFY(++t.crbegin() == t.crend());

    // Begin is one before end
    QVERIFY(t.begin() == --t.end());
    QVERIFY(t.cbegin() == --t.cend());
    QVERIFY(t.rbegin() == --t.rend());
    QVERIFY(t.crbegin() == --t.crend());

    // Delete all elements
    t.clear();

    // check allocator status
    QCOMPARE(CustomAllocator<binary_node<std::string>>::allocated.size(), 0);
    QCOMPARE(CustomAllocator<binary_node<std::string>>::total_allocated, 1);
    QCOMPARE(CustomAllocator<binary_node<std::string>>::total_deallocated, 1);

    // The tree is again empty
    QCOMPARE(t.size(), 0);
    QVERIFY(t.empty());

    // Iterators point again at the end
    QVERIFY(t.begin() == t.end());
    QVERIFY(t.cbegin() == t.cend());
    QVERIFY(t.rbegin() == t.rend());
    QVERIFY(t.crbegin() == t.crend());

    // Create by insert
    t.insert(t.begin(), "new element");
    QCOMPARE(*t.begin(), "new element");
    QCOMPARE(t, n("new element"));

    // check allocator status
    QCOMPARE(CustomAllocator<binary_node<std::string>>::allocated.size(), 1);
    QCOMPARE(CustomAllocator<binary_node<std::string>>::total_allocated, 2);
    QCOMPARE(CustomAllocator<binary_node<std::string>>::total_deallocated, 1);

    // The tree has 1 element again
    QCOMPARE(t.size(), 1);
    QVERIFY(!t.empty());
    QCOMPARE(t.max_size(), std::numeric_limits<std::size_t>::max());
}

void PreliminaryTest::typeRequirements() {
    // binary_tree<int>
    QVERIFY(std::is_default_constructible_v<binary_tree<int>>);
    QVERIFY(std::is_copy_constructible_v<binary_tree<int>>);
    QVERIFY(std::is_copy_assignable_v<binary_tree<int>>);
    QVERIFY(std::is_move_constructible_v<binary_tree<int>>);
    QVERIFY(std::is_move_assignable_v<binary_tree<int>>);
    QVERIFY(std::is_swappable_v<binary_tree<int>>);
    // binary_tree<Foo>
    QVERIFY(std::is_default_constructible_v<binary_tree<Foo>>);
    QVERIFY(std::is_copy_constructible_v<binary_tree<Foo>>);
    QVERIFY(std::is_copy_assignable_v<binary_tree<Foo>>);
    QVERIFY(std::is_move_constructible_v<binary_tree<Foo>>);
    QVERIFY(std::is_move_assignable_v<binary_tree<Foo>>);
    QVERIFY(std::is_swappable_v<binary_tree<Foo>>);
}

QTEST_MAIN(PreliminaryTest);
#include "PreliminaryTest.moc"
