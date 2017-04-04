#ifndef HF1631F76_C799_4BF8_B0D3_7A1A1A33A607
#define HF1631F76_C799_4BF8_B0D3_7A1A1A33A607

#include <TreeDS/tree.hpp>
#include "./lib/catch.hpp"

using namespace ds;

template <typename T>
void checkSize(const tree<T>& t, typename tree<T>::size_type s) {
	CHECK(t.size() == s);
	if (s == 0) {
		CHECK(t.empty());
		REQUIRE(t.begin() == t.end());
		REQUIRE(t.cbegin() == t.cend());
		REQUIRE(t.rbegin() == t.rend());
	} else {
		CHECK_FALSE(t.empty());
		REQUIRE_FALSE(t.begin() == t.end());
		REQUIRE_FALSE(t.cbegin() == t.cend());
		REQUIRE_FALSE(t.rbegin() == t.rend());
	}
	CHECK(t.size() < t.max_size());
}

template <typename TreeTest>
void checkTree(TreeTest treeTest) {
	checkSize(treeTest.t, treeTest.size);
	REQUIRE(treeTest.expected == treeTest.actual);
}

#endif /* HF1631F76_C799_4BF8_B0D3_7A1A1A33A607 */
