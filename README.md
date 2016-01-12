# TreeDS - work in progress
TreeDS (tree data structure) is a simple STL-like tree container type for C++11. It's a generic n-ary tree (every node can
have an arbitrary number of children).

## Features
* STL compatible
* No exernal dependecies
* Permissive licence
* Iterators provided: pre-order, post-order, ...
* Expandable with custom iterator types

## Overview
The data structure is based on the node class. Every node has a memory overhead of 5 pointers size (40 bytes on 64 bit systems) and the size of the stored object of course.
