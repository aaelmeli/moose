# Package Overview 
The Workbench Analysis Sequence Processor (WASP) was developed to streamline lexing, parsing, access, validation, and analysis of ascii text files.

The foundation of WASP resides on the parse tree data structure where each node in the tree represents the syntax of the input document. Nodes can parent nodes with children. 
Nodes that have no children are known as *terminal* or *leaf nodes* and represent Tokens (string, number, delimiter, etc.) in the text file.

The fast lexical analyzer generator (flex - https://www.gnu.org/software/flex/) and GNU Bison parser generator (https://www.gnu.org/software/bison/) are extensively used for lexing and parsing.

## Table of Contents

1. [Components](#components) - the primary packages in WASP
    * [Core](#core-package)
    * [Expr](#expression-engine)
    * [HIVE](#hive) 
    * [SIREN](#sequence-input-retrieval-engine-siren)
    * [SON](#standard-object-notation-son)
    * [DDI](#definition-driven-interpreter)
    * [EDDI](#eddi-input-interpreter)
    * [HALITE](#halite-engine)
    * [LSP](#language-server-protocol)
    * [Utils](#command-line-utilities)
2. [Getting Started](#getting-started) - code compilation requirements and instructions
    * [Requirements](#requirements)
    * [Code Configuration and Compilation](#code-configuration-and-compilation)
3. [Core](#core-package) - core datastructures
    * [String Pool](#string-pool)
    * [Token Pool](#token-pool)
    * [Tree Node Pool](#tree-node-pool)
    * [Interpreter](#interpreter)
4. [Expr](#expression-engine) - expression engine
    * [Arithmetic and Algabraic Operators](#arithmetic-and-algabraic-operators)
    * [Relational Operators](#relational-operators)
    * [Default Variables](#default-variables)
    * [Default Functions](#default-functions)
    * [Special Functions](#special-functions)
    * [Array Access](#array-access)
5. [HIVE](#hive) - HierarchicAL Input Validation Engine (HIVE)
    * [Overview](#hive)
    * [Input Validation Rules Summary](#input-validation-rules-summary)
        * [Input Validation Details and Examples](#input-validation-details-and-examples)
        * [Miscellaneous Details and Examples](#miscellaneous-details-and-examples)
        * [MinOccurs Details and Examples](#minoccurs-details-and-examples)
        * [MaxOccurs Details and Examples](#maxoccurs-details-and-examples)
        * [ValType Details and Examples](#valtype-details-and-examples)
        * [ValEnums Details and Examples](#valenums-details-and-examples)
        * [MinValInc Details and Examples](#minvalinc-details-and-examples)
        * [MaxValInc Details and Examples](#maxvalinc-details-and-examples)
        * [MinValExc Details and Examples](#minvalexc-details-and-examples)
        * [MaxValExc Details and Examples](#maxvalexc-details-and-examples)
        * [ExistsIn Details and Examples](#existsin-details-and-examples)
        * [NotExistsIn Details and Examples](#notexistsin-details-and-examples)
        * [SumOver Details and Examples](#sumover-details-and-examples)
        * [SumOverGroup Details and Examples](#sumovergroup-details-and-examples)
        * [IncreaseOver Details and Examples](#increaseover-details-and-examples)
        * [DecreaseOver Details and Examples](#decreaseover-details-and-examples)
        * [ChildAtMostOne Details and Examples](#childatmostone-details-and-examples)
        * [ChildExactlyOne Details and Examples](#childexactlyone-details-and-examples)
        * [ChildAtLeastOne Details and Examples](#childatleastone-details-and-examples)
        * [ChildCountEqual Details and Examples](#childcountequal-details-and-examples)
        * [ChildUniqueness Details and Examples](#childuniqueness-details-and-examples)
    * [Input Assistance Details](#input-assistance-details)
        * [MaxOccurs Assistance Details](#maxoccurs-assistance-details)
        * [ChildAtMostOne Assistance Details](#childatmostone-assistance-details)
        * [ChildExactlyOne Assistance Details](#childexactlyone-assistance-details)
        * [ValEnums Assistance Details](#valenums-assistance-details)
        * [ValType Assistance Details](#valtype-assistance-details)
        * [ExistsIn Assistance Details](#existsin-assistance-details)
        * [InputTmpl Assistance Details](#inputtmpl-assistance-details)
        * [InputName Assistance Details](#inputname-assistance-details)
        * [InputType Assistance Details](#inputtype-assistance-details)
        * [InputVariants Assistance Details](#inputvariants-assistance-details)
        * [InputDefault Assistance Details](#inputdefault-assistance-details)
        * [Description Assistance Details](#description-assistance-details) 
6. [SIREN](#sequence-input-retrieval-engine-siren) - Simple Input Retrieval ENgine (SIREN) 
    * [Selecting Nodes](#selecting-nodes)
    * [Selection Predicates](#predicates)
    * [Selecting Unknown Nodes](#selecting-unknown-nodes)
7. [SON](#standard-object-notation-son) - Standard Object Notation (SON) for general input data entry
8. [DDI](#definition-driven-interpreter) - Definition Driven Interpreter (DDI) for reduced input data entry  
9. [EDDI](#eddi-input-interpreter) - Extra Definition Driven Interpreter (EDDI) for reduced input data entry  
10. [HALITE](#halite-engine) - HierarchicAL Input Template Engine (HALITE) 
    * [Template Evaluation Summary](#template-evaluation-summary)
    * [Attributes and Expressions](#attributes-and-expressions)
        * [Intrinsic Attributes](#intrinsic-attributes)  
        * [Silent Attributes](#silent-attributes)
        * [Optional Attributes](#optional-attributes)
        * [Attribute Patterns](#attribute-patterns)
        * [Expressions](#expressions)
        * [Formatting](#formatting)
            * [Format Flags](#format-flags)
            * [Format Width](#format-width)
            * [Format Precision](#format-precision)
    * [Scoped Attribute](#scoped-attribute)
        * [Object Scoped Attribute](#object-scoped-attribute)
        * [Array Scoped Attribute](#object-scoped-attribute)
    * [File Imports](#file-imports)
        * [Parameterized File Imports](#parameterized-file-import)
            * [File Import using an Object](#file-import-using-an-object)
            * [Iterative File Import using an Array](#iterative-file-import-using-an-array-or-ranges)
    * [Conditional Blocks](#conditional-blocks)
11. [INI](#standard-object-notation-son) - configuration file format 
12. [LSP](#language-server-protocol) - Language Server Protocol support
    * [WASP Language Server](#wasp-language-server)
    * [Client](#client)
    * [LSPInterpreter](#lspinterpreter)
13. [Utils](#command-line-utilities) - utilities for command line file interaction (listing, validating, selecting, etc.)
    * [File Listing Utilities](#file-listing-utilities)
    * [File Component Selection Utilities](#file-component-selection-utilities)
    * [XML Utilities](#xml-utilities)
    * [File Validation](#file-validation-utilities)
    * [SON/DDI to JSON Utilities](#sonddi-to-json-utilities)
    * [HALITE](#the-hierarchal-input-template-expansion-engine)
    * [Schema Skeleton Creation Utility](#schema-skeleton-creation-utility)
    * [Autogenerate Input Documentation](#autogenerate-input-documentation)


# Components
WASP is composed of the following primary components:

1. [__Core__](#core-package): the waspcore package contains most necessary data structures and interface classes needed to interact with text files. 
    * StringPool: a string storage optimization class where ascii data are stored in a contiguous memory block where each string is null terminated and indexed.
    * TokenPool: a token/word storage optimization class where Token information (string data via StringPool, file location) are stored. Line and column are calculated on-the-fly via token file offset and file line offset.
    * TreeNodePool: a TreeNode storage class where TreeNode information (token, name, parent, type, children, etc.) are stored. 
    * Interpreter: an interface and high-level implementation class which facilitates specific grammar, lexer, and parser state information and parse tree storage
    * wasp_node: enumerated token/node types used to aid in identifying context and intent. 
    * utils: contains utility functions useful for string processing and tree visiting.
    * wasp_bug: contains software quality assurance and development aids that can be preprocessed out of deployments.
        * design by contract - insist, require, ensure, assert, check, remember.
        * timing: 3 levels of timers for code performance monitoring. 1-3, highest to lowest.
        * debug lines: set of macros that allow printing debug information to screen.
2. [__Expr__](#expression-engine): the waspexpr package contains lexer, parser, and evaluation logic for mathematical expressions.
    * Basic mathematical operators
        * multiplication '*'
        * division '/'
        * addition '+' 
        * subtraction '-'
        * boolean ('<','<=','==','!=', etc.) 
        * exponentiation '\^'
    * Scalar variable assignment, reference, and creation: known variable can be referenced and updated, or new variables created during expression evaluation.
    * Mathematical functions
3. [__HIT__](https://mooseframework.inl.gov/framework_development/sanity_checking.html): the [wasphit package](/wasphit/README.md) contains lexer, parser, and tree node view for the hit grammar.
4. [__HIVE__](#hive): the Hierarchical Input Validation Engine contains algorithms for validating a parse tree using a document schema/definition file, the flexible scalar and referential rules - supporting 
    * element occurrence,
    * value,
    * child uniqueness and choice,
    * existence,
    * sum,
    * predicated sum, etc.  
5. [__JSON__](http://www.json.org/): the waspjson package contains a lexer, as well as a parser for the JSON grammar (http://www.json.org/).
6. [__SIREN__](#sequence-input-retrieval-engine-siren): the Sequence Input Retrieval Engine (SIREN) contains a lexer, parser, and evaluation logic for tree node lookup; the flexible tree node lookup mechanism supports
    * absolute and relative wild-carded names and value, or
    * index-predicated node path lookup
7. [__SON__](#standard-object-notation-son): the Standard Object Notation (SON) - waspson package contains the lexer, parser, and tree node view for the SON grammar; the flexible, structured, input entry mechanism supports
    * blocks, objects, arrays, and keyed values, as well as
    * identified objects, arrays, and keyed values.
8. [__DDI__](waspddi/README.md#definition-driven-interpreter): the Definition-Driven Interpreter (DDI) contains a lexer, a parser, and an interpreter for the lightweight input format. 
    * Hierarchical Input format with very little syntax.
9. [__EDDI__](waspeddi/README.md#eddi-input-interpreter): the Extra Definition Driven Interpreter (EDDI) contains a lexer, a parser, and an interpreter for the lightweight input format. 
    * Hierarchical Input format with little syntax representing blocks, lists, sections, and scalar data.
    * Allows input field annotation which greatly enhances input validation capabilities 
10. [__HALITE__](#halite-engine): the Hierarchical Input Template Expansion engine provides a data-driven means of expanding patterned input.
    * Supports attribute and expression evaluations.
    * Supports template imports.
    * Supports conditional action blocks.
11. [__INI__](#standard-object-notation-son): configuration file format, supported by SON.
12. [__LSP__](#language-server-protocol): Language Server Protocol support
13. [__Utils__](#command-line-utilities) - the wasputils package contains executable utilities for listing/viewing, selecting, validating, and transforming WASP-supported grammars.
    * List: lists paths to each file element.
    * Select: allows using SIREN expression to select pieces of input.
    * Valid: validates a given text file with a given document definition/schema.
    * XML: translates a given text file into XML with data and location information.

# Getting Started    
For individuals wanting to compile the code from source, below are the tested requirements and configurations.


## Requirements
* C/CXX compiler (See repository .gitlab-ci.yml for build configurations)  
    * GCC-4.8 tested on Linux or Mac OS
    * LLVM-7.0.2 tested on Mac OS
    * Visual Studio 2012 for Windows
    * Intel 15 for Windows
    * MinGW 4.8.5 for Windows
* Git 1.7+
* CMake-2.8.12.2, 3.5, 3.8
* Python-2.7

## Code Configuration and Compilation
* Save the ssh-key in [code-int.ornl.gov](https://code-int.ornl.gov/profile/keys).
* Clone wasp `git clone git@code-int.ornl.gov:lefebvre/wasp.git ~/wasp`
* Change directory into wasp `cd ~/wasp`
* Initialize and update git submodules
`git submodule update --init`

* Create a build directory `mkdir -p ~/build/wasp`
* Change into the build `cd ~/build/wasp`
* Create a configuration script in ~/build/. Let's call it ../configure.sh(linux)

```
#!/bin/bash
# Linux bash file example
rm -rf CMake*
cmake \
 -D CMAKE_BUILD_TYPE:STRING=RELEASE \
 -D wasp_ENABLE_ALL_PACKAGES:BOOL=ON \
 -D wasp_ENABLE_TESTS:BOOL=ON \
 -D CMAKE_INSTALL_PREFIX=`pwd`/install \
 -G "Unix Makefiles" \
 ~/wasp
```
for example, to a script that will enable hit
```
#!/bin/bash
# Linux bash file example
rm -rf CMake*
cmake \
 -D CMAKE_BUILD_TYPE:STRING=RELEASE \
 -D wasp_ENABLE_wasphit=ON \
 -D wasp_ENABLE_TESTS:BOOL=ON \
 -D CMAKE_INSTALL_PREFIX=`pwd`/install \
 -G "Unix Makefiles" \
 ~/wasp
```

* Invoke configure script in the build directory.
`../configure.sh or ..\configure.bat`
  * It is recommended that the configure script in the build directory as opposed to the build/wasp directory because it allows the deletion of the build/wasp
directory without removing the configuration script.

Additionally, a script to enable a third part library (TPL) plugin, configure_tpl.sh :

```
cmake \
 -D wasp_ENABLE_ALL_PACKAGES:BOOL=ON \
 -D CMAKE_BUILD_TYPE:STRING=RELWITHDEBINFO \
 -D wasp_ENABLE_INSTALL_CMAKE_CONFIG_FILES:BOOL=ON \
 -D wasp_ENABLE_googletest:BOOL=OFF \
 -D wasp_ENABLE_testframework:BOOL=OFF \
 -D wasp_ENABLE_wasppy:BOOL=OFF \
 -D wasp_ENABLE_TESTS:BOOL=OFF \
 -D CMAKE_INSTALL_PREFIX=`pwd`/install \
 -D wasp_GENERATE_EXPORT_FILE_DEPENDENCIES:BOOL=ON \
 -D CMAKE_C_COMPILER:STRING=gcc \
 -D CMAKE_CXX_COMPILER:STRING=g++ \
 -VV \
 $*
 
```

Subsequently, invoke the script with a path to the root source:

```
../configure_tpl.sh /path/to/source
```

Lastly, due to an issue in cmake install file creation, a manual copy of the `waspConfig_install.cmake` is needed:

```
cp waspConfig_install.cmake install/lib/cmake/wasp/waspConfig.cmake
```

After configuration is complete, conduct the compilation via the make system available (make, NMake, Ninja, MSBuild, etc.)

