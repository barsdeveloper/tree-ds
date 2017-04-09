# TreeDS - work in progress
TreeDS (tree data structure) is an STL-like tree container type for C++11. It is a header only library that provides the most generic type of binary tree where every node can have at most two children.

Please feel free to modify it and contribute to the code if you want.

## Features
* STL compatible.
* No external dependecies.
* Permissive license.
* Iterators provided: pre-order, in-order, post-order
* Expandable with custom iterator types.

## Getting started
The library is header only, to start using it you just have to add the "include" directory to the include paths and then include it in your C++ file:

    #include <TreeDS/tree.hpp>

## Example
Here's a small example of utilization, for a more complete reference look at html doxygen-produced documentation (./html/index.html). You can merge and compile the code snippets.

```c++
#include <string>
#include <iostream>
#include <algorithm>
#include <TreeDS/tree.hpp>
using namespace std;

int main() {
    ds::tree<string> t; // empty tree created
    cout << t.size() << endl; // 0
```
      
you can add elements to the tree in the usual way
```c++
t.insert(t.begin(), "hello");
cout << "size: " << t.size() << " (" << *t.begin() << ")" << endl;
```

The most simple way to use this library is to construct trees inline. To to this just take a reference to tree::produce_node and call the operator () (function call operator). Look at the following example.

```c++
auto& n = ds::tree<int>::produce_node;
```

Now, calling n(some integer) will return a temporary_node<int> that can be passed to insert method or to the tree constructor itself. Let's create a more complex tree.

```c++
ds::tree<int> myTree(
    n(100)(         // root node
        nullptr,    // left child of root (no child)
        n(200)(     // right child of root
            n(300), // left child of 200
            n(400)  // right child of 200
        )
    )
);
/*
 * Result:
 *
 *   100
 *     \
 *      \
 *      200
 *       /\
 *      /  \
 *    300  400
 *
 */
```

You can use the same pattern also for the insert method, let's substitute the 300 with another tree:

```c++
myTree.insert(
    find(myTree.begin(), myTree.end(), 300), // find (first) position of node 300
    n(-10)(
        n(-20),
        n(-30)(
            n(-40)
        )
    )
  );
  /*
   * Result:
   *
   *   100
   *     \
   *      \
   *      200
   *       /\
   *      /  \
   *    -10  400
   *     /\
   *    /  \
   *  -20  -30
   *        /
   *       /
   *     -40
   */
```

Let's now iterate the tree in post-order and in-order. You can create a tree with a specified traversal algorithm type of an iterator. Remember pre_order is the default.

```c++
ds::tree<int, ds::in_order> inOrder(move(myTree));
```

We moved the tree content from myTree which now is empty, we could also copy the tree (same but without move) using copy constructor. That makes a deep copy (slow, avoid whenever you can).

```c++
cout << "In-order: ";
for (auto& value : inOrder) {
    cout << value << ", ";
}
cout << endl;
// In-order: 100, -20, -10, -40, -30, 200, 400,
```

You can always traverse a tree in a personalized manner, independently on the Algorithm parameter the tree has. Just construct the iterator by passing  tree.

```c++
cout << "Pre-order: ";
ds::tree<int>::iterator<ds::post_order> it(inOrder);
while(it != inOrder.end<ds::post_order>()) {
    cout << *it++ << ", "; // Pre-order: -20, -40, -30, -10, 400, 200, 100,
}
```
