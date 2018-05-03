#pragma once

#include <QtTest/QtTest>

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
        return !(*this == other);
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
        return !(*this == other);
    }
};

struct NotDefaultConstructuble {
    int value;
    NotDefaultConstructuble() = delete;
    NotDefaultConstructuble(int value) :
            value(value) {
    }
};