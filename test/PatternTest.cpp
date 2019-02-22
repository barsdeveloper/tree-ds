#include <QtTest/QtTest>

//#include <TreeDS/match>
#include <TreeDS/tree>

using namespace md;

class PatternTest : public QObject {

    Q_OBJECT

    private slots:
    void preliminary() {
        binary_tree<int> tree(n(12));
        //pattern p(tree);
    }
};

QTEST_MAIN(PatternTest);
#include "PatternTest.moc"
