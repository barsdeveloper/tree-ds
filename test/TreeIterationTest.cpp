#include <QtTest/QtTest>
#include <TreeDS/tree.hpp>
#include <algorithm>
#include <iterator>
#include <list>
#include <string>

using namespace std;
using namespace ds;

template <typename T> struct TreeTest {
  tree<T> t;
  typename tree<T>::size_type size;
  struct TraversalResult {
    list<T> pre_order;
    list<T> in_order;
    list<T> post_order;
  } expected;
};

Q_DECLARE_METATYPE(TreeTest<string>)
Q_DECLARE_METATYPE(tree<string>)
Q_DECLARE_METATYPE(tree<string>::size_type)
Q_DECLARE_METATYPE(list<string>)

temporary_node<string> n(const char *value) { return {value}; }

class TreeIterationTest : public QObject {

  Q_OBJECT;

private slots:
  void iteration();
  void iteration_data();
};

void TreeIterationTest::iteration() {
  QFETCH(tree<string>, t);
  QFETCH(int, expectedSize);
  QFETCH(list<string>, expectedPreOrder);
  QFETCH(list<string>, expectedInOrder);
  QFETCH(list<string>, expectedPostOrder);
  int actualSize = (int)t.size();
  list<string> actualPreOrder;
  list<string> actualInOrder;
  list<string> actualPostOrder;
  copy(t.begin<pre_order>(), t.end(), back_inserter(actualPreOrder));
  copy(t.begin<in_order>(), t.end<in_order>(), back_inserter(actualInOrder));
  copy(t.begin<post_order>(), t.end<post_order>(),
       back_inserter(actualPostOrder));
  QCOMPARE(expectedSize, actualSize);
  QCOMPARE(expectedPreOrder, actualPreOrder);
}

void TreeIterationTest::iteration_data() {

  QTest::addColumn<tree<string>>("t");
  QTest::addColumn<int>("expectedSize");
  QTest::addColumn<list<string>>("expectedPreOrder");
  QTest::addColumn<list<string>>("expectedInOrder");
  QTest::addColumn<list<string>>("expectedPostOrder");

  QTest::newRow("Empty String") << tree<string>(n("")) << 1 << list<string>{""}
                                << list<string>{""} << list<string>{""};

  QTest::newRow("Empty String")
      << tree<string>(n("1")(n("2"))) << 2 << list<string>{"1", "2"}
      << list<string>{"2", "1"} << list<string>{"2", "1"};

  QTest::newRow("Empty String")
      << tree<string>(n("1")(nullptr, n("2"))) << 2 << list<string>{"1", "2"}
      << list<string>{"1", "2"} << list<string>{"2", "1"};

  QTest::newRow("Empty String")
      << tree<string>(n("a")(
             n("b")(n("d")(n("h"),
                           nullptr // This can be omitted but I want to test it
                           ),
                    n("e")),
             n("c")(n("f")(n("j"), n("k")), n("g"))))
      << 10 << list<string>{"a", "b", "d", "h", "e", "c", "f", "j", "k", "g"}
      << list<string>{"h", "d", "b", "e", "a", "j", "f", "k", "c", "g"}
      << list<string>{"h", "d", "e", "b", "j", "k", "f", "g", "c", "a"};

  QTest::newRow("Empty String")
      << tree<string>(n("a")(
             n("b")(
                 n("c")(n("d")(n("e")(n("f"), n("g")), n("h")(nullptr, n("o"))),
                        n("i")(nullptr, n("n")(nullptr, n("p")))),
                 n("j")(nullptr, n("m")(nullptr, n("q")(nullptr, n("t"))))),
             n("k")(nullptr,
                    n("l")(nullptr, n("r")(nullptr, n("s")(nullptr, n("u")))))))
      << 21
      << list<string>{"a", "b", "c", "d", "e", "f", "g", "h", "o", "i", "n",
                      "p", "j", "m", "q", "t", "k", "l", "r", "s", "u"}
      << list<string>{"f", "e", "g", "d", "h", "o", "c", "i", "n", "p", "b",
                      "j", "m", "q", "t", "a", "k", "l", "r", "s", "u"}
      << list<string>{"f", "g", "e", "o", "h", "d", "p", "n", "i", "c", "t",
                      "q", "m", "j", "b", "u", "s", "r", "l", "k", "a"};

  QTest::newRow("Empty String")
      << tree<string>(n("1")(n("2")(n("3")(n("4")(n("5")(n("6")(n("7")(n("8")(n(
             "9")(n("10")(n("11")(n("12")(n("13")(n("14")(n("15"))))))))))))))))
      << 15 << list<string>{"1", "2",  "3",  "4",  "5",  "6",  "7", "8",
                            "9", "10", "11", "12", "13", "14", "15"}
      << list<string>{"15", "14", "13", "12", "11", "10", "9", "8",
                      "7",  "6",  "5",  "4",  "3",  "2",  "1"}
      << list<string>{"15", "14", "13", "12", "11", "10", "9", "8",
                      "7",  "6",  "5",  "4",  "3",  "2",  "1"};

  QTest::newRow("Empty String")
      << tree<string>(n("1")(
             nullptr,
             n("2")(
                 nullptr,
                 n("3")(
                     nullptr,
                     n("4")(
                         nullptr,
                         n("5")(
                             nullptr,
                             n("6")(
                                 nullptr,
                                 n("7")(
                                     nullptr,
                                     n("8")(
                                         nullptr,
                                         n("9")(
                                             nullptr,
                                             n("10")(
                                                 nullptr,
                                                 n("11")(
                                                     nullptr,
                                                     n("12")(
                                                         nullptr,
                                                         n("13")(
                                                             nullptr,
                                                             n("14")(
                                                                 nullptr,
                                                                 n("1"
                                                                   "5"))))))))))))))))
      << 15 << list<string>{"1", "2",  "3",  "4",  "5",  "6",  "7", "8",
                            "9", "10", "11", "12", "13", "14", "15"}
      << list<string>{"1", "2",  "3",  "4",  "5",  "6",  "7", "8",
                      "9", "10", "11", "12", "13", "14", "15"}
      << list<string>{"15", "14", "13", "12", "11", "10", "9", "8",
                      "7",  "6",  "5",  "4",  "3",  "2",  "1"};

  QTest::newRow("Empty String")
      << tree<string>(n("1")(n("2")(
             nullptr,
             n("3")(n("4")(
                 nullptr,
                 n("5")(n("6")(
                     nullptr,
                     n("7")(n("8")(
                         nullptr,
                         n("9")(n("10")(
                             nullptr,
                             n("11")(n("12")(
                                 nullptr,
                                 n("13")(n("14")(nullptr, n("15"))))))))))))))))
      << 15 << list<string>{"1", "2",  "3",  "4",  "5",  "6",  "7", "8",
                            "9", "10", "11", "12", "13", "14", "15"}
      << list<string>{"2",  "4",  "6", "8", "10", "12", "14", "15",
                      "13", "11", "9", "7", "5",  "3",  "1"}
      << list<string>{"15", "14", "13", "12", "11", "10", "9", "8",
                      "7",  "6",  "5",  "4",  "3",  "2",  "1"};
}

QTEST_MAIN(TreeIterationTest)
#include "TreeIterationTest.moc"
