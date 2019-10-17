# Debug support
This library comes with a limited debug support that allows the visualization of data structures. The debug is limited to console printing so it might not be suitable for large data structures. It may still be usefull and more efficient than using a debugger, especialy considering the support for address printing.

## Basic printing

```c++
#include <iostream>
#include <TreeDS/tree>
using namespace md;
int main() {
    nary_tree<char> t {
        n('a')(
            n('b'),
            n('c')(
                n('d'),
                n('e'),
                n('f'),
                n('g'),
                n('h'),
                n('i'),
                n('j'),
                n('k'),
                n('l')),
            n('m'),
            n('n'))};
    std::cout << t << std::endl;
}
```
```
n('a')(
    n('b'),
    n('c')(
        n('d'),
        n('e'),
        n('f'),
        n('g'),
        n('h'),
        n('i'),
        n('j'),
        ...),
    ...)

```

As we can see some nodes are hidden (...). We can control the number of nodes to print and other settings by setting some defines. The following options can be used:
```c++
#define MD_PRINT_TREE_MAX_NODES 10
#define MD_PRINT_TREE_INDENTATION 4
#define MD_PRINT_TREE_ADDRESS false
#define MD_PRINT_TREE_ADDRESS_DIGITS 4

#include <TreeDS/tree> // Defines must be set before including the header
```

## Examples
```c++
#include <iostream>

#define MD_PRINT_TREE_XXXXXXXXXXX XXXXXX // set a define before the include
#include <TreeDS/tree>
using namespace md;
int main() {
    nary_tree<char> t {
        n('a')(
            n('b'),
            n('c')(
                n('d'),
                n('e'),
                n('f'),
                n('g'),
                n('h'),
                n('i'),
                n('j'),
                n('k'),
                n('l')),
            n('m'),
            n('n'))};
    std::cout << t << std::endl;
}
```

#define MD_PRINT_TREE_MAX_NODES 14 | #define MD_PRINT_TREE_MAX_NODES 13 | #define MD_PRINT_TREE_MAX_NODES 2
---------------------------------- | ---------------------------------- | ---------------------------------
`n('a')(                         ` | `n('a')(                         ` | `n('a')(                         `
`    n('b'),                     ` | `    n('b'),                     ` | `    n('b'),                     `
`    n('c')(                     ` | `    n('c')(                     ` | `    ...)                        `
`        n('d'),                 ` | `        n('d'),                 `
`        n('e'),                 ` | `        n('e'),                 `
`        n('f'),                 ` | `        n('f'),                 `
`        n('g'),                 ` | `        n('g'),                 `
`        n('h'),                 ` | `        n('h'),                 `
`        n('i'),                 ` | `        n('i'),                 `
`        n('j'),                 ` | `        n('j'),                 `
`        n('k'),                 ` | `        n('k'),                 `
`        n('l')),                ` | `        n('l')),                `
`    n('m'),                     ` | `    n('m'),                     `
`    n('n'))                     ` | `    ...))                       `
