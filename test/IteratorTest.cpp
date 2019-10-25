#include <QtTest/QtTest>
#include <deque>
#include <list>
#include <memory> // std::unique_ptr
#include <queue>
#include <type_traits>
#include <unordered_map>
#include <vector>

#include <TreeDS/tree>

#include "Types.hpp"

using namespace std;
using namespace md;

template class md::tree_iterator<nary_tree<int>, policy::pre_order, node_navigator<nary_node<int>*>>;
template class md::tree_iterator<binary_tree<Bar>, policy::in_order, node_navigator<binary_node<Bar>*>>;

class IteratorTest : public QObject {

    Q_OBJECT

    private slots:
    void requirements();
    void test1();
    void test2();
    void test3();
};

void IteratorTest::requirements() {
    /*   ---   Default constructible   ---   */
    // Binary - elementary type
    QVERIFY(is_default_constructible_v<binary_tree<int>::iterator<policy::pre_order>>);
    QVERIFY(is_default_constructible_v<binary_tree<int>::iterator<policy::in_order>>);
    QVERIFY(is_default_constructible_v<binary_tree<int>::iterator<policy::post_order>>);
    QVERIFY(is_default_constructible_v<binary_tree<int>::iterator<policy::breadth_first>>);
    QVERIFY(is_default_constructible_v<binary_tree<int>::iterator<policy::leaves>>);
    QVERIFY(is_default_constructible_v<binary_tree<int>::iterator<policy::siblings>>);
    QVERIFY(is_default_constructible_v<binary_tree<int>::iterator<policy::fixed>>);
    // Binary - class type
    QVERIFY(is_default_constructible_v<binary_tree<vector<string>>::iterator<policy::pre_order>>);
    QVERIFY(is_default_constructible_v<binary_tree<vector<string>>::iterator<policy::in_order>>);
    QVERIFY(is_default_constructible_v<binary_tree<vector<string>>::iterator<policy::post_order>>);
    QVERIFY(is_default_constructible_v<binary_tree<vector<string>>::iterator<policy::breadth_first>>);
    QVERIFY(is_default_constructible_v<binary_tree<vector<string>>::iterator<policy::leaves>>);
    QVERIFY(is_default_constructible_v<binary_tree<vector<string>>::iterator<policy::siblings>>);
    QVERIFY(is_default_constructible_v<binary_tree<vector<string>>::iterator<policy::fixed>>);
    // Nary - elementary type
    QVERIFY(is_default_constructible_v<nary_tree<float*>::iterator<policy::pre_order>>);
    QVERIFY(is_default_constructible_v<nary_tree<float*>::iterator<policy::in_order>>);
    QVERIFY(is_default_constructible_v<nary_tree<float*>::iterator<policy::post_order>>);
    QVERIFY(is_default_constructible_v<nary_tree<float*>::iterator<policy::breadth_first>>);
    QVERIFY(is_default_constructible_v<nary_tree<float*>::iterator<policy::leaves>>);
    QVERIFY(is_default_constructible_v<nary_tree<float*>::iterator<policy::siblings>>);
    QVERIFY(is_default_constructible_v<nary_tree<float*>::iterator<policy::fixed>>);
    // Nary - class type
    QVERIFY(is_default_constructible_v<nary_tree<std::list<char>>::iterator<policy::pre_order>>);
    QVERIFY(is_default_constructible_v<nary_tree<std::list<char>>::iterator<policy::in_order>>);
    QVERIFY(is_default_constructible_v<nary_tree<std::list<char>>::iterator<policy::post_order>>);
    QVERIFY(is_default_constructible_v<nary_tree<std::list<char>>::iterator<policy::breadth_first>>);
    QVERIFY(is_default_constructible_v<nary_tree<std::list<char>>::iterator<policy::leaves>>);
    QVERIFY(is_default_constructible_v<nary_tree<std::list<char>>::iterator<policy::siblings>>);
    QVERIFY(is_default_constructible_v<nary_tree<std::list<char>>::iterator<policy::fixed>>);

    /*   ---   Copy constructible   ---   */
    // Binary - elementary type
    QVERIFY(is_copy_constructible_v<binary_tree<double>::iterator<policy::pre_order>>);
    QVERIFY(is_copy_constructible_v<binary_tree<double>::iterator<policy::in_order>>);
    QVERIFY(is_copy_constructible_v<binary_tree<double>::iterator<policy::post_order>>);
    QVERIFY(is_copy_constructible_v<binary_tree<double>::iterator<policy::breadth_first>>);
    QVERIFY(is_copy_constructible_v<binary_tree<double>::iterator<policy::leaves>>);
    QVERIFY(is_copy_constructible_v<binary_tree<double>::iterator<policy::siblings>>);
    QVERIFY(is_copy_constructible_v<binary_tree<double>::iterator<policy::fixed>>);
    // Binary - class type
    QVERIFY(is_copy_constructible_v<binary_tree<unique_ptr<list<string>>>::iterator<policy::pre_order>>);
    QVERIFY(is_copy_constructible_v<binary_tree<unique_ptr<list<string>>>::iterator<policy::in_order>>);
    QVERIFY(is_copy_constructible_v<binary_tree<unique_ptr<list<string>>>::iterator<policy::post_order>>);
    QVERIFY(is_copy_constructible_v<binary_tree<unique_ptr<list<string>>>::iterator<policy::breadth_first>>);
    QVERIFY(is_copy_constructible_v<binary_tree<unique_ptr<list<string>>>::iterator<policy::leaves>>);
    QVERIFY(is_copy_constructible_v<binary_tree<unique_ptr<list<string>>>::iterator<policy::siblings>>);
    QVERIFY(is_copy_constructible_v<binary_tree<unique_ptr<list<string>>>::iterator<policy::fixed>>);
    // Nary - elementary type
    QVERIFY(is_copy_constructible_v<nary_tree<char>::iterator<policy::pre_order>>);
    QVERIFY(is_copy_constructible_v<nary_tree<char>::iterator<policy::in_order>>);
    QVERIFY(is_copy_constructible_v<nary_tree<char>::iterator<policy::post_order>>);
    QVERIFY(is_copy_constructible_v<nary_tree<char>::iterator<policy::breadth_first>>);
    QVERIFY(is_copy_constructible_v<nary_tree<char>::iterator<policy::leaves>>);
    QVERIFY(is_copy_constructible_v<nary_tree<char>::iterator<policy::siblings>>);
    QVERIFY(is_copy_constructible_v<nary_tree<char>::iterator<policy::fixed>>);
    // Nary - class type
    QVERIFY(is_copy_constructible_v<nary_tree<deque<int>>::iterator<policy::pre_order>>);
    QVERIFY(is_copy_constructible_v<nary_tree<deque<int>>::iterator<policy::in_order>>);
    QVERIFY(is_copy_constructible_v<nary_tree<deque<int>>::iterator<policy::post_order>>);
    QVERIFY(is_copy_constructible_v<nary_tree<deque<int>>::iterator<policy::breadth_first>>);
    QVERIFY(is_copy_constructible_v<nary_tree<deque<int>>::iterator<policy::leaves>>);
    QVERIFY(is_copy_constructible_v<nary_tree<deque<int>>::iterator<policy::siblings>>);
    QVERIFY(is_copy_constructible_v<nary_tree<deque<int>>::iterator<policy::fixed>>);

    /*   ---   Copy assignable   ---   */
    // Binary - elementary type
    QVERIFY(is_copy_assignable_v<binary_tree<void*>::iterator<policy::pre_order>>);
    QVERIFY(is_copy_assignable_v<binary_tree<void*>::iterator<policy::in_order>>);
    QVERIFY(is_copy_assignable_v<binary_tree<void*>::iterator<policy::post_order>>);
    QVERIFY(is_copy_assignable_v<binary_tree<void*>::iterator<policy::breadth_first>>);
    QVERIFY(is_copy_assignable_v<binary_tree<void*>::iterator<policy::leaves>>);
    QVERIFY(is_copy_assignable_v<binary_tree<void*>::iterator<policy::siblings>>);
    QVERIFY(is_copy_assignable_v<binary_tree<void*>::iterator<policy::fixed>>);
    // Binary - class type
    QVERIFY((is_copy_assignable_v<binary_tree<NonCopyable>::iterator<policy::pre_order>>));
    QVERIFY((is_copy_assignable_v<binary_tree<NonCopyable>::iterator<policy::in_order>>));
    QVERIFY((is_copy_assignable_v<binary_tree<NonCopyable>::iterator<policy::post_order>>));
    QVERIFY((is_copy_assignable_v<binary_tree<NonCopyable>::iterator<policy::breadth_first>>));
    QVERIFY((is_copy_assignable_v<binary_tree<NonCopyable>::iterator<policy::leaves>>));
    QVERIFY((is_copy_assignable_v<binary_tree<NonCopyable>::iterator<policy::siblings>>));
    QVERIFY((is_copy_assignable_v<binary_tree<NonCopyable>::iterator<policy::fixed>>));
    // Nary - elementary type
    QVERIFY(is_copy_assignable_v<nary_tree<bool>::iterator<policy::pre_order>>);
    QVERIFY(is_copy_assignable_v<nary_tree<bool>::iterator<policy::in_order>>);
    QVERIFY(is_copy_assignable_v<nary_tree<bool>::iterator<policy::post_order>>);
    QVERIFY(is_copy_assignable_v<nary_tree<bool>::iterator<policy::breadth_first>>);
    QVERIFY(is_copy_assignable_v<nary_tree<bool>::iterator<policy::leaves>>);
    QVERIFY(is_copy_assignable_v<nary_tree<bool>::iterator<policy::siblings>>);
    QVERIFY(is_copy_assignable_v<nary_tree<bool>::iterator<policy::fixed>>);
    // Nary - class type
    QVERIFY(is_copy_assignable_v<nary_tree<NonCopyable>::iterator<policy::pre_order>>);
    QVERIFY(is_copy_assignable_v<nary_tree<NonCopyable>::iterator<policy::in_order>>);
    QVERIFY(is_copy_assignable_v<nary_tree<NonCopyable>::iterator<policy::post_order>>);
    QVERIFY(is_copy_assignable_v<nary_tree<NonCopyable>::iterator<policy::breadth_first>>);
    QVERIFY(is_copy_assignable_v<nary_tree<NonCopyable>::iterator<policy::leaves>>);
    QVERIFY(is_copy_assignable_v<nary_tree<NonCopyable>::iterator<policy::siblings>>);
    QVERIFY(is_copy_assignable_v<nary_tree<NonCopyable>::iterator<policy::fixed>>);

    /*   ---   Destructible   ---   */
    // Binary - elementary type
    QVERIFY(is_destructible_v<binary_tree<int*>::iterator<policy::pre_order>>);
    QVERIFY(is_destructible_v<binary_tree<int*>::iterator<policy::in_order>>);
    QVERIFY(is_destructible_v<binary_tree<int*>::iterator<policy::post_order>>);
    QVERIFY(is_destructible_v<binary_tree<int*>::iterator<policy::breadth_first>>);
    QVERIFY(is_destructible_v<binary_tree<int*>::iterator<policy::leaves>>);
    QVERIFY(is_destructible_v<binary_tree<int*>::iterator<policy::siblings>>);
    QVERIFY(is_destructible_v<binary_tree<int*>::iterator<policy::fixed>>);
    // Binary - class type
    QVERIFY(is_destructible_v<binary_tree<unique_ptr<list<string>>>::iterator<policy::pre_order>>);
    QVERIFY(is_destructible_v<binary_tree<unique_ptr<list<string>>>::iterator<policy::in_order>>);
    QVERIFY(is_destructible_v<binary_tree<unique_ptr<list<string>>>::iterator<policy::post_order>>);
    QVERIFY(is_destructible_v<binary_tree<unique_ptr<list<string>>>::iterator<policy::breadth_first>>);
    QVERIFY(is_destructible_v<binary_tree<unique_ptr<list<string>>>::iterator<policy::leaves>>);
    QVERIFY(is_destructible_v<binary_tree<unique_ptr<list<string>>>::iterator<policy::siblings>>);
    QVERIFY(is_destructible_v<binary_tree<unique_ptr<list<string>>>::iterator<policy::fixed>>);
    // Nary - elementary type
    QVERIFY(is_destructible_v<nary_tree<bool>::iterator<policy::pre_order>>);
    QVERIFY(is_destructible_v<nary_tree<bool>::iterator<policy::in_order>>);
    QVERIFY(is_destructible_v<nary_tree<bool>::iterator<policy::post_order>>);
    QVERIFY(is_destructible_v<nary_tree<bool>::iterator<policy::breadth_first>>);
    QVERIFY(is_destructible_v<nary_tree<bool>::iterator<policy::leaves>>);
    QVERIFY(is_destructible_v<nary_tree<bool>::iterator<policy::siblings>>);
    QVERIFY(is_destructible_v<nary_tree<bool>::iterator<policy::fixed>>);
    // Nary - class type
    QVERIFY(is_destructible_v<nary_tree<queue<int>>::iterator<policy::pre_order>>);
    QVERIFY(is_destructible_v<nary_tree<queue<int>>::iterator<policy::in_order>>);
    QVERIFY(is_destructible_v<nary_tree<queue<int>>::iterator<policy::post_order>>);
    QVERIFY(is_destructible_v<nary_tree<queue<int>>::iterator<policy::breadth_first>>);
    QVERIFY(is_destructible_v<nary_tree<queue<int>>::iterator<policy::leaves>>);
    QVERIFY(is_destructible_v<nary_tree<queue<int>>::iterator<policy::siblings>>);
    QVERIFY(is_destructible_v<nary_tree<queue<int>>::iterator<policy::fixed>>);
}

void IteratorTest::test1() {
    nary_tree<int, policy::post_order> tree;
    decltype(tree)::iterator<policy::post_order> unrelated;
    auto start = tree.begin();
    decltype(tree)::iterator<policy::post_order> related(start);

    QVERIFY(unrelated == related);
    QVERIFY(related == unrelated);
    QVERIFY(!(unrelated != related));
    QVERIFY(!(related != unrelated));

    QVERIFY(start == related);
    QVERIFY(related == start);
    QVERIFY(!(start != related));
    QVERIFY(!(related != start));
}

void IteratorTest::test2() {
    using iterator_t       = binary_tree<Bar>::iterator<policy::in_order>;
    using const_iterator_t = binary_tree<Bar>::const_iterator<policy::in_order>;

    binary_tree<Bar> tree(n(1, 2)(n(2, 3), n(3, 4)));

    // Obtained using std::find.
    iterator_t it1 = std::find(tree.begin(policy::in_order()), tree.end(policy::in_order()), Bar(1, 2));
    // Copy constructed
    iterator_t it2(it1);
    // Copy assignment (below).
    iterator_t it3;
    it3 = it1;
    // Unmodifiable reference to a pointer.
    const iterator_t& const_ref = it1;

    ++it2; // it2 == it1 + 1
    --it3; // it3 == it1 - 1

    // Constant iterator copy constructed.
    const_iterator_t itc1(it1);
    // Contant iterator copy assigned (below).
    const_iterator_t itc2;
    itc2 = it2;

    // 3 iterators, all different each other.
    QVERIFY(it1 != it2 && it2 != it3 && it3 != it2);

    // Compare agains value
    QCOMPARE(*it1, Bar(1, 2));
    QCOMPARE(*it2, Bar(3, 4));
    QCOMPARE(*it3, Bar(2, 3));
    QCOMPARE(*itc1, Bar(1, 2));
    QCOMPARE(*itc2, Bar(3, 4));

    // Indirect method call (through iterator).
    QCOMPARE(it1->method(), "mutable");
    QCOMPARE(itc1->method(), "constant");
    QCOMPARE(const_ref->method(), "constant");
    QCOMPARE((*it1).method(), "mutable");
    QCOMPARE((*itc1).method(), "constant");
    QCOMPARE((*const_ref).method(), "constant");

    // Direct method call (thorugh iterator->node->value->method).
    QCOMPARE(it1.get_raw_node()->get_value().method(), "mutable");
    QCOMPARE(itc1.get_raw_node()->get_value().method(), "constant");
    QCOMPARE(const_ref.get_raw_node()->get_value().method(), "constant");

    // Iterator equals constant itself.
    QVERIFY(it1 == itc1);
    QVERIFY(itc1 == it1);
    QVERIFY(!(it1 != itc1));
    QVERIFY(!(itc1 != it1));

    // Different iterators are not equal.
    QVERIFY(it1 != it2);
    QVERIFY(it2 != it1);
    QVERIFY(!(it1 == it2));
    QVERIFY(!(it2 == it1));

    // Iterator equals constant itself (using it2).
    QVERIFY(it2 == itc2);
    QVERIFY(itc2 == it2);
    QVERIFY(!(it2 != itc2));
    QVERIFY(!(itc2 != it2));

    // Different constant iterators are not equal.
    QVERIFY(itc1 != itc2);
    QVERIFY(itc2 != itc1);
    QVERIFY(!(itc1 == itc2));
    QVERIFY(!(itc2 == itc1));
}

void IteratorTest::test3() {
    const nary_tree<char> tree {
        n('a')(
            n('b')(
                n('e'),
                n('f')(
                    n('g'))),
            n('c')(
                n('g'),
                n('h')),
            n('d'))};

    auto it_r = tree.root();
    QCOMPARE(*it_r, 'a');
    ++it_r;
    QCOMPARE(it_r.get_raw_node(), nullptr);
    ++it_r;
    QCOMPARE(*it_r, 'a');

    auto it_1 = tree.begin();
    it_1.go_last_child();
    QCOMPARE(*it_1, 'd');

    it_1.go_parent();
    QCOMPARE(*it_1, 'a');

    it_1.go_last_child();
    QCOMPARE(*it_1, 'd');

    it_1.go_parent();
    QCOMPARE(*it_1, 'a');

    auto it_2 = it_1.other_policy(policy::pre_order()).go_child(1);
    QCOMPARE(*it_2, 'c');

    it_2.go_prev_sibling();
    QCOMPARE(*it_2, 'b');

    it_2.go_last_child();
    QCOMPARE(*it_2, 'f');

    // Enough, now let's go wild
    auto it_3
        = it_2
              .other_policy(policy::breadth_first())
              .go_first_child()
              .other_policy(policy::fixed())
              .go_parent()
              .go_parent()
              .other_policy(policy::post_order())
              .go_next_sibling()
              .go_next_sibling()
              .go_parent();
    QCOMPARE(*it_3, 'a');

    auto it_4
        = it_3
              .other_policy(policy::siblings())
              .go_child(0)
              .go_next_sibling()
              .other_policy(policy::pre_order())
              .go_prev_sibling()
              .go_first_child()
              .other_policy(policy::breadth_first());
    QCOMPARE(*it_4, 'e');
}

QTEST_MAIN(IteratorTest)
#include "IteratorTest.moc"
