# Core Package
The core package of WASP provides the foundation for text processing. A primary mission of WASP is to capture information in a manner that facilitates reconstituting a users' text file. This information is captured in two primary stages: lexing and parsing of the text files:

* Lexical analysis processes the text file by recognizing patterns of text and producing a token that encapsulates information pertaining to this pattern.
* The parser recognizes patterns of tokens and constructs a parse tree for future examination. 

Typically, all whitespace are not captured in the parse tree as they can be deduced and reconstituted upon request. 

## Core Parts

The primary components of wasp core are:

1. StringPool: provides contiguous string storage. All strings are stored, null-terminated, in a single data container.
2. TokenPool: provides contiguous token storage. Tokens consist of string data, a token type, and a file byte offset. 
    * An index is used to identify the location in the StringPool where the data resides. 
    * The token pool stores new line offsets.
    * The file byte offset is used to compute line and column with the assistance of the new line offset.
3. TreeNodePool: manages TreeNodes, each of which consist of a node name, type, and child indices. 
    * Uses StringPool for contiguous node name storage.
    * Uses TokenPool to store all leaf node token information.
4. Interpreter: base class providing boilerplate lexing and parsing logic.
    * Contains core token and node construction logic for lexer and parser to use.
5. Format: utility methods for formatting values which provide a type-safe printf as needed by [expression engine](/waspexpr/README.md#expression-engine) and [HALITE](/wasphalite/README.md#hierarchical-input-validation-engine-hive).
6. wasp_node: central location for node and token type enumeration.
7. Object: generic type data structure to facilitate typed-data access to hierarchical data; facilitates Halite data-driven capabilities.
    
Each Pool and its subsequent Interpreter is a templated class allowing space consolidation when application size is known.
For example, if the application is to interpreter files that will never be more than 65KiB, an `unsigned short` can be used as the template type.

## String Pool
The string pool consists of two members: (1) a vector of chars and (2) a vector of indices indicating string starts. This ensures that the string data that is consumed from a text file are reasonably maintained and that storage size is not inflated. 

In a benchmark of one application's input - consisting of 300MB in which the document tokens' mathematical mode was 3 characters, with a mean of 4, 
using `std::string` produces on average ~28+ byte overhead per token, or, specifically, 8 byte heap pointer, 8 byte size, 8 byte heap page header, and 8 byte heap memory page. 

In contrast, the StringPool only requires a 5 byte overhead per token, or, specifically, a 4-byte index and a null terminating character. Using the StringPool facilitates a significant memory consolidation.


## Token Pool
Token pool associates a token type and a file byte offset with the text that resides in the StringPool. New lines are a special piece of meta data that is captured for text location deduction.

The type information indicates whether it is an integer, real, word, declarator, terminator, etc. This information can be used to deduce context or perform operations on a class of data.

The file byte offset is the absolute location in the file at which the text begins. This is not intuitive to a user, but when combined with new line offset text location, line and column, can be deduced.

Specifically, the line can be computed as the distance from the upper bound of the token's file offset in the list of line file offsets and the line offset.

`line = distance( line_file_offsets.begin(), line_file_offsets.upperbound( token_file_offset ) )`


The column can be computed as the difference of the token's file offset and the upper bound, minus 1, of the token index into the list of line file offsets.

`column = token_file_offset - ( line_file_offsets.upperbound(token_file_offset) - 1)`


## Tree Node Pool
The tree node pool coordinates access and storage to nodes. Nodes can be classified as inner and leaf nodes. 
Inner nodes can have children and represent a set of input. Leaf nodes always represent a token. For example, `key = 3.14159` has the following hierarchy:
```
document 
|_keyed_value 
  |_ declarator ('key')
  |_ assign ('=')
  |_ value  (3.14159)
```
Here, the inner node is 'keyed_value' with leaf child nodes `declarator`, `assign`, and `value`. 

All nodes have associated meta data:

1. Node type: declarator, terminator, value, name, etc.
2. Node name: user familiar name.
3. Parent node pool index: the location in the tree node pool that the parent of the node resides.

Additional meta data for parent/inner nodes consists of:

1. First child pool index: which is the index of the first child.
2. The number of children.

There is a convenient TreeNodeView class that provides consolidated per-node data access.

## Interpreter
The interpreter is the base class to facilitate all syntax specific interpreters ([DDI](/waspddi/README.md#definition-driven-interpreter), [SON](/waspson/README.md#standard-object-notation-son), etc.). 
The interpreter brokers transactions between the lexer and parser and also stages and stores the parse tree for future access.

The interpreter manages the TreeNodePool and tracks the root of the parse tree. 
It also provides a stage construct to facilitate text syntax where hierarchy is ambiguous and sub-trees may not exist to immediate parent (see [DDI](/waspddi/README.md#definition-driven-interpreter) for active use).


