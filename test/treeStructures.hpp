#ifndef H69BCE67F_885F_4F07_90A0_E78159828481
#define H69BCE67F_885F_4F07_90A0_E78159828481

#include <TreeDS/tree>
#include <TreeDS/iterator/pre_order.hpp>
#include <list>

using namespace std;
using namespace ds;

template <typename T>
struct TraversalResult {
	list<T> preOrder;
	list<T> inOrder;
	list<T> postOrder;
};

template <typename T>
struct TreeTest {
	tree<T> t;
	typename tree<T>::size_type size;
	TraversalResult<T> expected;
	TraversalResult<T> actual;
};

auto& n = tree<string>::produce_node;

auto stringTrees = [](){
	return list<TreeTest<string>> {
		{
			// Empty tree
			{},
			0,
			{}
		},
		{
			// Tree containing only a root with value "1"
			{n("1")},
			1,
			{{"1"}, {"1"}, {"1"}}
		},
		{
			// As before but with empty string, just in case
			{n("")},
			1,
			{{""}, {""}, {""}}
		},
		{
			{ // A root with a left child
				n("1")(
						n("2")
				)
			},
			2,
			{
				{"1", "2"},
				{"2", "1"},
				{"2", "1"}
			}
		},
		{
			{ // A root with a right child
				n("1")(
						nullptr,
						n("2")
				)
			},
			2,
			{
				{"1", "2"},
				{"1", "2"},
				{"2", "1"}
			}
		},
		{
			{ // Small tree
				n("a")(
						n("b")(
								n("d")(
										n("h"),
										nullptr // This can be omitted but I want to test it
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
				{"a", "b", "d", "h", "e", "c", "f", "j", "k", "g"},
				{"h", "d", "b", "e", "a", "j", "f", "k", "c", "g"},
				{"h", "d", "e", "b", "j", "k", "f", "g", "c", "a"}
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
				{"a", "b", "c", "d", "e", "f", "g", "h", "o", "i", "n", "p", "j", "m", "q", "t", "k", "l", "r", "s", "u"},
				{"f", "e", "g", "d", "h", "o", "c", "i", "n", "p", "b", "j", "m", "q", "t", "a", "k", "l", "r", "s", "u"},
				{"f", "g", "e", "o", "h", "d", "p", "n", "i", "c", "t", "q", "m", "j", "b", "u", "s", "r", "l", "k", "a"}
			}
		},
		{
			{
				// Line tree having where every internal node have only left child
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
				{"1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15"},
				{"15", "14", "13", "12", "11", "10", "9", "8", "7", "6", "5", "4", "3", "2", "1"},
				{"15", "14", "13", "12", "11", "10", "9", "8", "7", "6", "5", "4", "3", "2", "1"}
			}
		},
		{
			{
				// Line tree having where every internal node have only right child
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
				{"1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15"},
				{"1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15"},
				{"15", "14", "13", "12", "11", "10", "9", "8", "7", "6", "5", "4", "3", "2", "1"}
			}
		},
		{
			{
				// Zig Zag tree
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
				{"1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15"},
				{"2", "4", "6", "8", "10", "12", "14", "15", "13", "11", "9", "7", "5", "3", "1"},
				{"15", "14", "13", "12", "11", "10", "9", "8", "7", "6", "5", "4", "3", "2", "1"}
			}
		}
	};
};

#endif /* H69BCE67F_885F_4F07_90A0_E78159828481 */
