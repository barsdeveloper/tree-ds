# TreeDS - work in progress
TreeDS (tree data structure) is an STL-like tree container type for C++11. It is a header only library that provides the most generic type of tree where every node can have an arbitrary number of children (and the maximum number of children can't be set). It provides a double interface: a container-centric (STL-like) one and a node-centric (classic) one.

Please feel free to modify it and contribute to the code if you want.

## Features
* STL compatible.
* No external dependecies.
* Permissive licence.
* Iterators provided: pre-order, ...
* Expandable with custom iterator types.
* Boost compatibile serialization.

## Getting started
The library is header only, to start using it you just have to add the "include" directory to the include paths and then include it in your C++ file:

    #include <TreeDS/tree.hpp>

## Documentation
Code examples and documentation is available in the wiki: https://github.com/barsan-ds/TreeDS/wiki
