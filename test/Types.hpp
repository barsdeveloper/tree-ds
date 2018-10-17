#pragma once

#include <QtTest/QtTest>
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
