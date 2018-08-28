![Security](https://sonarcloud.io/api/project_badges/measure?project=tree-ds&metric=security_rating)
![Readability](https://sonarcloud.io/api/project_badges/measure?project=tree-ds&metric=reliability_rating)
![Mantainability](https://sonarcloud.io/api/project_badges/measure?project=tree-ds&metric=sqale_rating)
![Coverage](https://sonarcloud.io/api/project_badges/measure?project=tree-ds&metric=coverage)

# TreeDS - work in progress
TreeDS (tree data structure) is an STL-like tree container library for C++17 that provides two types of trees: nary and
binary.

Please feel free to modify it and contribute to the code if you want.

## Features
* Standard C++17 (except ```#pragma once```, hopefully will be replaced with modules).
* STL compatible.
* No external dependecies.
* Permissive license.
* Iterators provided: pre-order, in-order, post-order
* Expandable with custom iterator types.
* Tree pattern matching (conceptually similar to Regex).

## Getting started
The library is header only, to start using it you just have to add the "./include" directory to the include paths and
then `#include` it in your .cpp file:

```c++
#include <TreeDS/tree.hpp>
```

## Example
Here's a small example of utilization (you can merge and compile the code snippets).

```c++
#include <string>
#include <iostream>
#include <algorithm>
#include <TreeDS/tree.hpp>
using namespace std;

int main() {
    ds::tree<string> t; // empty tree created
    cout << t.size_value() << endl; // 0
```
      
Add elements to the tree in the usual way:
```c++
t.insert(t.begin(), "hello");
// size_value: 1 (hello)
cout << "size: " << t.size_value() << " (" << *t.begin() << ")" << endl;
```

The simplest way to use this library is to create anonymous nodes. Use the function ```n(something)``` to obtain a
`struct_node<something_t>`, then use the function call operator to add (and replace) children to a node. The value
obtained has a complex type, don't to refer to it directly, just feed it to the insert/emplace method or the tree
constructor itself. Look at the following example.

```c++
ds::tree<int> myTree(
    n(100)(             // root node
        n(),            // left child of root (it is empty: no child)
        n(200)(         // right child of root
            n(300)(     // left child of 200
                n(500), // left child of 300
                n(600)  // right child of 300
            ),
            n(400)      // right child of 200
        )
    )
);
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

Use the same pattern also for the insert method, let's substitute the node 300 (the whole subtree):

```c++
myTree.insert(
    find(myTree.begin(), myTree.end(), 300), // iterator to the (first) position of node 300
    n(-10)(
        n(-20),
        n(-30)(
            n(-40)
        )
    )
  );
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

Let's now iterate the tree. You can create a tree with a specified traversal algorithm by settings the second template
parameter. This algorithm will be the default choice (used in the range based loops, for example).

```c++
ds::tree<int, ds::in_order> inOrder(move(myTree)); // mytree is empty at this point
//            ^^^^^^^^^^^^
```

We moved the content from `myTree` (which now is empty) to `inOrder`, we could also copy the tree using copy
constructor. That makes a deep copy (**slow**, avoid whenever you can).

```c++
// In-order: 100, -20, -10, -40, -30, 200, 400,
cout << "In-order: ";
for (auto& value : inOrder) {
    cout << value << ", ";
}
cout << endl;
```

You can always traverse a tree in a personalized manner, independently on the Algorithm parameter the tree has. Just
get an iterator by passing a tree as argument.

```c++
// Post-order: -20, -40, -30, -10, 400, 200, 100, 
cout << "Post-order: ";
ds::tree<int>::iterator<ds::post_order> it(inOrder);
// alternatively auto it = inOrder.begin<post_order>();
while(it != inOrder.end<ds::post_order>()) {
    cout << *it++ << ", ";
}
}// end of main
```
