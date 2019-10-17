#include <QtTest/QtTest>
#include <iostream>

#define MD_PRINT_TREE_MAX_NODES 2 // set a define before the include
#include <TreeDS/tree>

using namespace md;

class DebugTest : public QObject {

    Q_OBJECT

    private slots:
    void test() {
        nary_tree<char> t {
            n('a')(
                n('b'),
                n('c')(
                    n('d'),
                    n('e'),
                    n('f'),
                    n('g'),
                    n('h'),
                    n('i'),
                    n('j'),
                    n('k'),
                    n('l')),
                n('m'),
                n('n'))};
        std::cout << t << std::endl;
    }
};

QTEST_MAIN(DebugTest);
#include "DebugTest.moc"
