#include <QtTest/QtTest>

#include <TreeDS/binary_tree>

#include "Types.hpp"

using namespace std;
using namespace ds;

Q_DECLARE_METATYPE(binary_tree<Foo>);

class TreeTest : public QObject {

    Q_OBJECT
};

QTEST_MAIN(TreeTest);
#include "TreeTest.moc"
