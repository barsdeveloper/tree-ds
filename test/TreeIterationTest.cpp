#include <QtTest/QtTest>
#include <TreeDS/tree.hpp>
#include <iterator>
#include <list>
#include <map>
#include <string>

using namespace std;
using namespace ds;

template <typename T>
struct TreeTest {
    tree<T> t;
    typename tree<T>::size_type size;
    map<std::string, list<T>> expected;
};

Q_DECLARE_METATYPE(TreeTest<string>)
Q_DECLARE_METATYPE(tree<string>)
Q_DECLARE_METATYPE(tree<string>::size_type)
Q_DECLARE_METATYPE(list<string>)

class TreeIterationTest : public QObject {

    Q_OBJECT;

private slots:
    void preOrder_data() {
        QTest::addColumn<tree<string>>("tree");
        QTest::addColumn<tree<string>::size_type>("expectedSize");
        QTest::addColumn<list<string>>("expectedResult");
        int i = 0;
        for (const auto& test : data()) {
            QTest::newRow(to_string(i++).data())
                << test.t
                << test.size
                << test.expected.at("PreOrder");
        }
    }

    void preOrder() {
        QFETCH(tree<string>, t);
        QFETCH(tree<string>::size_type, expectedSize);
        QFETCH(list<string>, expectedResult);
        list<string> actualResult;
        tree<string>::size_type actualSize = t.size();
        copy(t.begin<pre_order>(), t.end(), back_inserter(actualResult));
        QCOMPARE(expectedSize, actualSize);
        QCOMPARE(expectedSize, actualResult.size());
        QCOMPARE(expectedResult, actualResult);
    }

public:
    static list<TreeTest<string>> data() {
        // clang-format off
        return {
            {// Empty tree
                {},
                0,
                {}
            },
            {// Tree containing only a root with value "1"
                {n("1")},
                1,
                {
                    {"PreOrder", {"1"}},
                    {"InOrder", {"1"}},
                    {"PostOrder", {"1"}}
                }
            },
            {// As before but with empty string, just in case
                {n("")},
                1,
                {
                    {"PreOrder", {""}},
                    {"InOrder", {""}},
                    {"PostOrder", {""}}
                }
            },
            {
                {// A root with a left child
                    n("1")(
                        n("2")
                    )
                },
                2,
                {
                    {"PreOrder", {"1", "2"}},
                    {"InOrder", {"2", "1"}},
                    {"PostOrder", {"2", "1"}}
                }
            },
            {
                {// A root with a right child
                    n("1")(
                        nullptr,
                        n("2")
                    )
                },
                2,
                {
                    {"PreOrder", {"1", "2"}},
                    {"InOrder", {"1", "2"}},
                    {"PostOrder", {"2", "1"}}
                }
            },
            {
                {// Small tree
                    n("a")(
                        n("b")(
                            n("d")(
                                n("h"),
                                nullptr// This can be omitted but I want to test it
                            ),
                            n("e")
                        ),
                        n("c")(
                            n("f")(
                                n("j"),
                                n("k")
                            ),
                            n("g")
                        )
                    )
                },
                10,
                {
                    {"PreOrder", {"a", "b", "d", "h", "e", "c", "f", "j", "k", "g"}},
                    {"InOrder", {"h", "d", "b", "e", "a", "j", "f", "k", "c", "g"}},
                    {"PostOrder", {"h", "d", "e", "b", "j", "k", "f", "g", "c", "a"}}
                }
            },
            {
                {
                    n("a")(
                        n("b")(
                            n("c")(
                                n("d")(
                                    n("e")(
                                        n("f"),
                                        n("g")
                                    ),
                                    n("h")(
                                        nullptr,
                                        n("o")
                                    )
                                ),
                                n("i")(
                                    nullptr,
                                    n("n")(
                                        nullptr,
                                        n("p")
                                    )
                                )
                            ),
                            n("j")(
                                nullptr,
                                n("m")(
                                    nullptr,
                                    n("q")(
                                        nullptr,
                                        n("t")
                                    )
                                )
                            )
                        ),
                        n("k")(
                            nullptr,
                            n("l")(
                                nullptr,
                                n("r")(
                                    nullptr,
                                    n("s")(
                                        nullptr,
                                        n("u")
                                    )
                                )
                            )
                        )
                    )
                },
                21,
                {
                    {"PreOrder", {"a", "b", "c", "d", "e", "f", "g", "h", "o", "i", "n", "p", "j", "m", "q", "t", "k", "l", "r", "s", "u"}},
                    {"InOrder", {"f", "e", "g", "d", "h", "o", "c", "i", "n", "p", "b", "j", "m", "q", "t", "a", "k", "l", "r", "s", "u"}},
                    {"PostOrder", {"f", "g", "e", "o", "h", "d", "p", "n", "i", "c", "t", "q", "m", "j", "b", "u", "s", "r", "l", "k", "a"}}
                }
            },
            {
                {// Line tree having where every internal node have only left child
                    n("1")(
                        n("2")(
                            n("3")(
                                n("4")(
                                    n("5")(
                                        n("6")(
                                            n("7")(
                                                n("8")(
                                                    n("9")(
                                                        n("10")(
                                                            n("11")(
                                                                n("12")(
                                                                    n("13")(
                                                                        n("14")(
                                                                            n("15")
                                                                        )
                                                                    )
                                                                )
                                                            )
                                                        )
                                                    )
                                                )
                                            )
                                        )
                                    )
                                )
                            )
                        )
                    )
                },
                15,
                {
                    {"PreOrder", {"1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15"}},
                    {"InOrder", {"15", "14", "13", "12", "11", "10", "9", "8", "7", "6", "5", "4", "3", "2", "1"}},
                    {"PostOrder", {"15", "14", "13", "12", "11", "10", "9", "8", "7", "6", "5", "4", "3", "2", "1"}}
                }
            },
            {
                {// Line tree having where every internal node have only right child
                    n("1")(
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
                                                                            n("15")
                                                                        )
                                                                    )
                                                                )
                                                            )
                                                        )
                                                    )
                                                )
                                            )
                                        )
                                    )
                                )
                            )
                        )
                    )
                },
                15,
                {
                    {"PreOrder", {"1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15"}},
                    {"InOrder", {"1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15"}},
                    {"PostOrder", {"15", "14", "13", "12", "11", "10", "9", "8", "7", "6", "5", "4", "3", "2", "1"}}
                }
            },
            {
                {// Zig Zag tree
                    n("1")(
                        n("2")(
                            nullptr,
                            n("3")(
                                n("4")(
                                    nullptr,
                                    n("5")(
                                        n("6")(
                                            nullptr,
                                            n("7")(
                                                n("8")(
                                                    nullptr,
                                                    n("9")(
                                                        n("10")(
                                                            nullptr,
                                                            n("11")(
                                                                n("12")(
                                                                    nullptr,
                                                                    n("13")(
                                                                        n("14")(
                                                                            nullptr,
                                                                            n("15")
                                                                        )
                                                                    )
                                                                )
                                                            )
                                                        )
                                                    )
                                                )
                                            )
                                        )
                                    )
                                )
                            )
                        )
                    )
                },
                15,
                {
                    {"PreOrder", {"1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15"}},
                    {"InOrder", {"2", "4", "6", "8", "10", "12", "14", "15", "13", "11", "9", "7", "5", "3", "1"}},
                    {"PostOrder", {"15", "14", "13", "12", "11", "10", "9", "8", "7", "6", "5", "4", "3", "2", "1"}}
                }
            }
        };
        // clang-format on
    }
};

QTEST_MAIN(TreeIterationTest)
