#pragma once

#include <QtTest/QtTest>

struct Foo {
    int a;
    int b;
    Foo() = default;
    Foo(int a, int b) :
            a(a),
            b(b) {
    }
};

struct Bar {
    int c;
    int d;
    Bar() = default;
    Bar(int c, int d) :
            c(c),
            d(d) {
    }
};

Q_DECLARE_METATYPE(Foo);
Q_DECLARE_METATYPE(Bar);
