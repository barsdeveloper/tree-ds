![Security](https://sonarcloud.io/api/project_badges/measure?project=tree-ds&metric=security_rating)
![Readability](https://sonarcloud.io/api/project_badges/measure?project=tree-ds&metric=reliability_rating)
![Mantainability](https://sonarcloud.io/api/project_badges/measure?project=tree-ds&metric=sqale_rating)
![Coverage](https://sonarcloud.io/api/project_badges/measure?project=tree-ds&metric=coverage)

# TreeDS - work in progress
TreeDS (tree data structure) is an STL-like tree container library for C++17 that provides two types of trees: nary and binary.

Please feel free to modify it and contribute to the code if you want.

## Features
* Standard C++17 (except ```#pragma once```, hopefully will be replaced with modules).
* STL compatible.
* No external dependecies.
* Permissive license.
* Iterators provided: pre-order, in-order, post-order, breadth-first
* Expandable with custom iterator types.
* Tree pattern matching (conceptually similar to regex).

## Design goals
This library is designed with **performance** in mind. It will avoid expensive features like virtual member functions, memory allocations and type erarsure whenever possible, in general it follows the style of the standard library that prefers template parameters over runtime entities. Consider that I have a decently good understanding of C++ mechanics yet I'm far from being an expert.

To sum up this library is pretty interesting in what it can do, maybe more complex that the average tree library but it offers a stupid simple and intuitive interface.

## Getting started
The library is header only, to start using it you just have to add the "./include" directory to the include paths and
then `#include` it in your .cpp file:

```c++
#include <TreeDS/tree>
```

## Example
Here's a small example of utilization (you can merge and compile the code snippets).

```c++
#include <string>
#include <iostream>
#include <algorithm>
#include <TreeDS/tree.hpp>
using namespace std;
using ds::n;

int main() {
    ds::nary_tree<string> t;
    cout << t.size_value() << endl; // 0
```

We just created an empty nary_tree: a type of tree where each node can have an arbitrary number of children.

Add elements to the tree in the usual way:

```c++
t.insert(t.begin(), "hello");
// size: 1 (hello)
cout << "size: " << t.size_value() << " (" << *t.begin() << ")" << endl;
```

The simplest way to use this library is to create anonymous nodes. Use the function ```n(something)``` to obtain a `struct_node<something_t>`, then use the function call operator to add (and replace) children to a node. The value obtained has a complex type, don't to refer to it directly, just feed it to the constructor itself. Look at the following example.

```c++
ds::binary_tree<int> myTree(
    n(100)(              // root node
        n(),             // left child of root (it is empty: no child)
        n(200)(          // right child of root
            n(300)(      // left child of 200
                n(500),  // left child of 300
                n(600)), // right child of 300
            n(400))));   // right child of 200
/* Result:
 *
 *     100
 *       \
 *        \
 *        200
 *         /\
 *        /  \
 *      300  400
 *      /\
 *     /  \
 *   500  600
 */
```

Use the same approach also for the insert method, let's substitute the node 300 (the whole subtree):

```c++
myTree.insert(
    // find returns iterator to the (first) position of node 300
    find(myTree.begin(), myTree.end(), 300),
    n(-10)(
        n(-20),
        n(-30)(
            n(-40))));
  /* Result:
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

Let's now iterate the tree. You can create a tree with a specified traversal policy by setting the second template parameter. This algorithm will be the default choice (used in the range based loops, for example).

```c++
ds::binary_tree<int, ds::in_order> inOrderTree(move(myTree));
//            ^^^^^^^^^^^^ default policy used to iterate the tree
```

At this point moved the content from `myTree` (which now is empty) to `inOrder`, we could also copy the tree using copy constructor. That makes a deep copy (**slow**, avoid whenever you can).

```c++
// In-order: 100, -20, -10, -40, -30, 200, 400,
cout << "In-order: ";
for (auto& value : inOrder) {
    cout << value << ", ";
}
cout << endl;
```

You can always traverse a tree in a personalized manner, independently on the Policy parameter the tree has. Just call the `begin` method with a specific policy template parameter.

```c++
// Post-order: -20, -40, -30, -10, 400, 200, 100, 
cout << "Post-order: ";
result << "Post-order: ";
auto it = inOrderTree.begin<ds::post_order>();
while (it != inOrderTree.end<ds::post_order>()) {
    result << *it++ << ", ";
}
}// end of main
```
