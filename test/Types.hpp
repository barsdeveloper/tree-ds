#pragma once

#include <QtTest/QtTest>
#include <algorithm>
#include <deque>
#include <memory>
#include <string>

struct NonCopyable {
    char a;
    int b;
    NonCopyable()                   = delete;
    NonCopyable(const NonCopyable&) = delete;
    NonCopyable(NonCopyable&&)      = delete;
    NonCopyable(char a, int b) :
            a(a),
            b(b) {
    }
    NonCopyable& operator=(const NonCopyable&) = delete;
    NonCopyable& operator=(NonCopyable&&) = delete;
    bool operator==(const NonCopyable& other) const {
        return this->a == other.a && this->b == other.b;
    }
    bool operator!=(const NonCopyable& other) const {
        return !this->operator==(other);
    }
};

struct Foo {
    int a;
    int b;
    Foo(int a, int b) :
            a(a),
            b(b) {
    }
    bool operator==(const Foo& other) const {
        return a == other.a && b == other.b;
    }
    bool operator!=(const Foo& other) const {
        return !this->operator==(other);
    }
};

struct Bar {
    int c;
    int d;
    Bar(int c, int d) :
            c(c),
            d(d) {
    }
    bool operator==(const Bar& other) const {
        return c == other.c && d == other.d;
    }
    bool operator!=(const Bar& other) const {
        return !this->operator==(other);
    }
    std::string method() const {
        return "constant";
    }
    std::string method() {
        return "mutable";
    }
};

struct ConvertibleFrom {
    std::string value;
    ConvertibleFrom(std::string value) :
            value(value) {
    }
    bool operator==(const ConvertibleFrom& other) const {
        return this->value == other.value;
    }
    bool operator!=(const ConvertibleFrom& other) const {
        return !this->operator==(other);
    }
};

struct Target {
    std::string value;
    Target(std::string value) :
            value(value) {
    }
    Target(ConvertibleFrom other) :
            value(other.value) {
    }
    bool operator==(const Target& other) const {
        return this->value == other.value;
    }
    bool operator!=(const Target& other) const {
        return !this->operator==(other);
    }
};

struct ConvertibleTo {
    std::string value;
    ConvertibleTo(std::string value) :
            value(value) {
    }
    bool operator==(const ConvertibleTo& other) const {
        return this->value == other.value;
    }
    bool operator!=(const ConvertibleTo& other) const {
        return !this->operator==(other);
    }
    operator Target() const {
        return {value};
    }
};

template <typename T>
struct CustomAllocator {
    static std::deque<T*> allocated;
    static int total_allocated;
    static int total_deallocated;
    using value_type  = T;
    CustomAllocator() = default;
    template <typename Other>
    CustomAllocator(const CustomAllocator<Other>&) {
    }
    CustomAllocator(const CustomAllocator&) = default;
    T* allocate(std::size_t count) {
        T* ptr = static_cast<T*>(::operator new(count * sizeof(T)));
        allocated.push_back(ptr);
        ++total_allocated;
        return ptr;
    }
    void deallocate(T* ptr, std::size_t) {
        assert(ptr != nullptr);
        assert(
            std::find(allocated.begin(), allocated.end(), ptr)
            != allocated.end());
        allocated.erase(std::remove(allocated.begin(), allocated.end(), ptr), allocated.end());
        ++total_deallocated;
        ::operator delete(ptr);
    }
    bool operator==(const CustomAllocator&) const {
        return true;
    }
    bool operator!=(const CustomAllocator&) const {
        return false;
    }
};

template <typename T>
std::deque<T*> CustomAllocator<T>::allocated;

template <typename T>
int CustomAllocator<T>::total_allocated = 0;

template <typename T>
int CustomAllocator<T>::total_deallocated = 0;
