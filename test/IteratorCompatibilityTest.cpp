#include <QtTest/QtTest>
#include <TreeDS/tree.hpp>
#include <new>
#include "Types.hpp"

using namespace std;
using namespace ds;

template <typename T>
struct Allocator1 {
    using value_type = T;
    T* allocate(std::size_t n) {
        return ::operator new(n);
    }
    void deallocate(T* p, std::size_t n) {
        ::operator delete(p);
    }
};

template <typename T>
struct Allocator2 {
    using value_type = T;
    T* allocate(std::size_t n) {
        return ::operator new(n);
    }
    void deallocate(T* p, std::size_t n) {
        ::operator delete(p);
    }
};

class IteratorCompatibilityTest : public QObject {

    Q_OBJECT;

private:
    tree<int> a;
    tree<int, pre_order> b{n(78)};

private slots:
    void initTestCase() {
    }
    void differentIterator() {
    }
};

QTEST_MAIN(IteratorCompatibilityTest);
#include "IteratorCompatibilityTest.moc"
