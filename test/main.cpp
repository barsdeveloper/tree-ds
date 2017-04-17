
#define CATCH_CONFIG_MAIN
#define CATCH_CONFIG_COLOUR_NONE // eclipse Neon doesn't support ANSI escape codes coloring, try without and see if works

#include <iostream>
#include <functional>
#include <algorithm>

#include <TreeDS/tree>
#include <TreeDS/iterator/pre_order.hpp>
#include <TreeDS/iterator/in_order.hpp>
#include <TreeDS/iterator/post_order.hpp>

#include "./lib/catch.hpp"
#include "checkFunctions.hpp"
#include "treeStructures.hpp"

using namespace ds;
using namespace std;

TEST_CASE("Tree iterators") {
	SECTION("String trees test") {
		for(auto& testStructure : stringTrees()) {
			using Tree = decltype(testStructure.t);
			using T = decltype(testStructure.t)::value_type;
			using Size = decltype(testStructure.t)::size_type;

			Tree& t = testStructure.t;
			Size& size = testStructure.size;

			Tree copy{t}; // copy into a new tree
			SECTION("copy a tree") {
				checkSize(copy, size);
				REQUIRE(copy == t);
				REQUIRE(t == copy);
			}

			auto move(std::move(copy)); // move to a new tree
			SECTION("Move a tree") {
				checkSize(move, size);
				checkSize(copy, 0);
				REQUIRE(move == t);
				REQUIRE(t == move);
			}
			// compute iterators
			for(auto& val : tree<T, pre_order>(t)) {
				testStructure.actual.preOrder.push_back(val);
			}
			for(auto& val : tree<T, in_order>(t)) {
				testStructure.actual.inOrder.push_back(val);
			}
			for(auto& val : tree<T, post_order>(t)) {
				testStructure.actual.postOrder.push_back(val);
			}
			checkTree(testStructure);
		}
	}
}

TEST_CASE("Tree insertion") {
	auto& n = tree<string>::produce_node;
	tree<string, in_order> t{
		n("e")(
				n("c")(
						n("a")(
								nullptr,
								n("b")
						),
						n("d")
				),
				n("f")(
						nullptr,
						n("h")(
								n("g"),
								n("i")
						)
				)
		)
	};
	{
		list<string> traverseResult;
		for(auto& val : t) traverseResult.push_back(val);
		REQUIRE((traverseResult == list<string>{"a", "b", "c", "d", "e", "f", "g", "h", "i"}));
	}
	t.insert(
		find(begin(t), end(t), "h"),
		n("2")(
				n("1"),
				n("3")
		)
	);
	{
		list<string> traverseResult;
		for(auto& val : t) traverseResult.push_back(val);
		REQUIRE((traverseResult == list<string>{"a", "b", "c", "d", "e", "f", "1", "2", "3"}));
	}
}
