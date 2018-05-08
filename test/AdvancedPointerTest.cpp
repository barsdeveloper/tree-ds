#include <QtTest/QtTest>
#include <TreeDS/advanced_pointer.hpp>

class AdvancedPointerTest : public QObject {

    Q_OBJECT;

private slots:
    void plainPointerCreation() {

        // Create a pointer to int
        ds::advanced_pointer<int, false, false> pointer;

        QCOMPARE(pointer.get(), nullptr);

        int value      = 999;
        int* effective = &value;
        pointer.set(effective);
        QCOMPARE(pointer.get(), &value);
        QCOMPARE(*pointer.get(), 999);
    }
};

QTEST_MAIN(AdvancedPointerTest);
#include "AdvancedPointerTest.moc"