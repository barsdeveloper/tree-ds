
#define CATCH_CONFIG_MAIN
#define CATCH_CONFIG_COLOUR_NONE // eclipse Neon doesn't support ANSI escape codes coloring, try without and see if works
#pragma GCC diagnostic ignored "-Wparentheses" // try withous and see if works anyway

#include <iostream>
#include <functional>

#include <TreeDS/tree>
#include <TreeDS/iterator/pre_order.hpp>

#include "./lib/catch.hpp"
#include "checkFunctions.hpp"
#include "treeStructures.hpp"

using namespace ds;
using namespace std;

TEST_CASE("Tree test") {
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
			tree<T, pre_order> preOrderTree(t);
			for(auto& val : preOrderTree) {
				testStructure.actual.preOrder.push_back(val);
			}
			checkTree(testStructure);
		}
	}
}
