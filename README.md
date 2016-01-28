# TreeDS - work in progress
TreeDS (tree data structure) is an STL-like tree container type for C++11. It is a header only library that provides the most generic type of tree where every node can have an arbitrary number of children (and the maximum number of children can't be set).

## Features
* STL compatible.
* No exernal dependecies.
* Permissive licence.
* Iterators provided: pre-order, ...
* Expandable with custom iterator types.
* Boost compatibile serialization.

## Overview
The data structure is based on the node class. Every node has a memory overhead of 5 pointers size (40 bytes on 64 bit systems) plus the size of the stored object, of course.
