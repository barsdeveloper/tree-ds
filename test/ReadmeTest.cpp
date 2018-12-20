#include <QtTest/QtTest>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>

#include <TreeDS/tree>

using namespace std;
using md::n;

class ReadmeTest : public QObject {

    Q_OBJECT

    private slots:
    void test() {
        md::nary::tree<string> t; // empty tree created
        QCOMPARE(t.size(), 0);

        {
            t.insert(t.begin(), "hello");
            std::stringstream result;
            result << "size: " << t.size() << " (" << *t.begin() << ")";
            QCOMPARE(result.str(), "size: 1 (hello)");
        }

        md::binary::tree<int> myTree(
            n(100)(              // root node
                n(),             // left child of root (it is empty: no child)
                n(200)(          // right child of root
                    n(300)(      // left child of 200
                        n(500),  // left child of 300
                        n(600)), // right child of 300
                    n(400))));   // right child of 200

        myTree.insert(
            find(myTree.begin(), myTree.end(), 300), // iterator to the (first) position of node 300
            n(-10)(
                n(-20),
                n(-30)(
                    n(-40))));

        md::binary::tree<int, md::in_order> inOrderTree(move(myTree));
        //            ^^^^^^^^^^^^ default algorithm used to iterate the tree

        {
            std::stringstream cout;
            // In-order: 100, -20, -10, -40, -30, 200, 400,
            cout << "In-order: ";
            for (auto& value : inOrderTree) {
                cout << value << ", ";
            }
            QCOMPARE(cout.str(), "In-order: 100, -20, -10, -40, -30, 200, 400, ");
        }

        {
            std::stringstream cout;
            cout << "Post-order: ";
            auto it = inOrderTree.begin<md::post_order>();
            while (it != inOrderTree.end<md::post_order>()) {
                cout << *it++ << ", ";
            }
            QCOMPARE(cout.str(), "Post-order: -20, -40, -30, -10, 400, 200, 100, ");
        }
    }
};

QTEST_MAIN(ReadmeTest);
#include "ReadmeTest.moc"
