![Security](https://sonarcloud.io/api/project_badges/measure?project=barsan-md_tree-ds&metric=security_rating)
![Readability](https://sonarcloud.io/api/project_badges/measure?project=barsan-md_tree-ds&metric=reliability_rating)
![Mantainability](https://sonarcloud.io/api/project_badges/measure?project=barsan-md_tree-ds&metric=sqale_rating)
![Coverage](https://sonarcloud.io/api/project_badges/measure?project=barsan-md_tree-ds&metric=coverage)
![Coverage](https://sonarcloud.io/api/project_badges/measure?project=barsan-md_tree-ds&metric=coverage)
![Duplication](https://sonarcloud.io/api/project_badges/measure?project=barsan-md_tree-ds&metric=duplicated_lines_density)

# TreeDS
TreeDS (tree data structure) is an STL-like tree container library for C++17 that provides two types of trees: nary and binary.

Please feel free to open issues for any question and contribute to the code if you want.

## Features
* Standard C++17 (except ```#pragma once```, hopefully will be replaced with modules).
* STL compatible.
* No external dependecies.
* Permissive license.
* Iterators provided: pre-order, in-order (```binary_tree``` only), post-order, breadth-first
* Expandable with custom iterator types.
* Tree pattern matching (conceptually similar to regex). Still a work in progress.

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
#include <TreeDS/tree>
#include <TreeDS/view> // needed only to use views
using namespace std;
using md::n;

int main() {
    md::nary_tree<string> t;
    cout << t.size_value() << endl; // 0
```

We just created an empty ```nary_tree```: a type of tree where each node can have an arbitrary number of children.

Add elements to the tree in the usual way:

```c++
t.insert(t.begin(), "hello");
// size: 1 (hello)
cout << "size: " << t.size_value() << " (" << *t.begin() << ")" << endl;
```

The simplest way to use this library is to create anonymous nodes. Use the function ```n(something)``` to obtain a `struct_node<something_t>`, then use the function call operator to add (and replace) children to a node. The value obtained has a complex type, don't to refer to it directly, just feed it to the constructor itself. Look at the following example.

```c++
md::binary_tree<int> myTree(
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

Use the same approach also for the insert method, let's substitute the node 300 (the whole subtree) with another subtree:

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
md::binary_tree<int, md::policy::in_order> inOrderTree(move(myTree));
//                   ^^^^^^^^^^^^ default policy used to iterate the tree
```

We just moved the content from `myTree` (which now is empty) to `inOrderTree`, we could also copy the tree using copy constructor. That makes a deep copy (**slow**, avoid whenever you can).

```c++
// In-order: 100, -20, -10, -40, -30, 200, 400,
cout << "In-order: ";
for (auto& value : inOrder) {
    cout << value << ", ";
}
cout << endl;
```

You can always traverse a tree in a personalized manner, independently on the Policy parameter the tree has. Just call the `begin` method with a specific policy argument.

```c++
// Post-order: -20, -40, -30, -10, 400, 200, 100, 
cout << "Post-order: ";
auto it = inOrderTree.begin(md::policy::post_order());
while (it != inOrderTree.end(md::policy::post_order()) {
    cout << *it++ << ", ";
}
cout << endl;
```

Or create a view of the tree and specify the policy of it.

```c++
// Breadth-first: 100, 200, -10, 400, -20, -30, -40,
md::binary_tree_view<int, md::policy::breadth_first> view(inOrderTree);
cout << "Breadth-first: ";
for (auto& value : view) {
    cout << value;
}
cout << endl;
}// end of main
```

`nary_tree_view<T>` and `binary_tree_view<T>` are coneptually similar to STL's `string_view`: they create a "view" (read only) that is a part of some bigger data structure.
Views can also refer to a subtree (take as root a node which is not the root of the original tree) and iterated coherently.
Views are cheap to copy so please use them whenever deep copying a tree is something that must be avoided.
