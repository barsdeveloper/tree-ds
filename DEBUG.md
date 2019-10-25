# Debug support
This library comes with a limited debug support that allows the visualization of data structures. The debug is limited to console printing so it might not be suitable for large data structures. It may still be usefull and more immediate than using a debugger, especialy considering the support for address printing.

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

As we can see some nodes are hidden (...). We can control the number of nodes to print and other settings by means of defines. The following options can be used:
```c++
#define MD_PRINT_TREE_MAX_NODES 10
#define MD_PRINT_TREE_INDENTATION 4
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
### Nodes count
<table>
    <tr>
        <th colspan="4"><pre>#define MD_PRINT_TREE_MAX_NODES X</pre></th>
    </tr>
    <tr>
        <th>14</th>
        <th>13</th>
        <th>2</th>
        <th>1</th>
    </tr>
    <tr>
        <td>
            <pre>n('a')(
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
    n('n'))</pre>
        </td>
        <td>
            <pre>n('a')(
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
    ...)</pre>
        </td>
        <td>
            <pre>n('a')(
    n('b'),
    ...)</pre>
        </td>
        <td>
            <pre>n('a')(
    ...)</pre>
        </td>
    </tr>
</table>

### Indentation
<table>
    <tr>
        <th colspan="4"><pre>#define MD_PRINT_TREE_INDENTATION X</pre></th>
    </tr>
    <tr>
        <th>0</th>
        <th>2</th>
        <th>4</th>
        <th>8</th>
    </tr>
    <tr>
        <td>
            <pre>n('a')(
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
...)</pre>
        </td>
        <td>
            <pre>n('a')(
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
  ...)</pre>
        </td>
        <td>
            <pre>n('a')(
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
    ...)</pre>
        </td>
        <td>
            <pre>n('a')(
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
        ...)</pre>
        </td>
    </tr>
</table>

### Address
<table>
    <tr>
        <th colspan="4"><pre>#define MD_PRINT_TREE_ADDRESS_DIGITS X</pre></th>
    </tr>
    <tr>
        <th>0</th>
        <th>1</th>
        <th>2</th>
        <th>8</th>
    </tr>
    <tr>
        <td>
            <pre>n('a')(
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
    ...)</pre>
        </td>
        <td>
            <pre>
n('a' @0)(
    n('b' @0),
    n('c' @0)(
        n('d' @0),
        n('e' @0),
        n('f' @0),
        n('g' @0),
        n('h' @0),
        n('i' @0),
        n('j' @0),
        ...),
    ...)</pre>
        </td>
        <td>
            <pre>
n('a' @c0)(
    n('b' @c0),
    n('c' @0)(
        n('d' @a0),
        n('e' @e0),
        n('f' @20),
        n('g' @60),
        n('h' @a0),
        n('i' @e0),
        n('j' @20),
        ...),
    ...)</pre>
        </td>
        <td>
            <pre>
n('a' @336c27c0)(
    n('b' @336c57c0),
    n('c' @336c5800)(
        n('d' @336c5ea0),
        n('e' @336c5ee0),
        n('f' @336c5f20),
        n('g' @336c5f60),
        n('h' @336c5fa0),
        n('i' @336c5fe0),
        n('j' @336c6020),
        ...),
    ...)</pre>
        </td>
    </tr>
</table>
