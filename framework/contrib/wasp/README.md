# Package Overview 
The Workbench Analysis Sequence Processor (WASP) was developed to streamline lexing, parsing, access, validation, and analysis of ascii text files.

The foundation of WASP resides on the parse tree data structure where each node in the tree represents the syntax of the input document. Nodes can parent nodes with children. 
Nodes that have no children are known as *terminal* or *leaf nodes* and represent Tokens (string, number, delimiter, etc.) in the text file.

The fast lexical analyzer generator (flex - https://www.gnu.org/software/flex/) and GNU Bison parser generator (https://www.gnu.org/software/bison/) are extensively used for lexing and parsing.

## Table of Contents

1. [Components](#components) - the primary packages in WASP
    * [Core](/waspcore/README.md#core-package)
    * [Expr](/waspexpr/README.md#expression-engine)
    * [HIVE](/wasphive/README.md#hive) 
    * [SIREN](/waspsiren/README.md#sequence-input-retrieval-engine-siren)
    * [SON](/waspson/README.md#standard-object-notation-son)
    * [DDI](/waspddi/README.md#definition-driven-interpreter)
    * [EDDI](/waspeddi/README.md#eddi-input-interpreter)
    * [HALITE](/wasphalite/README.md#halite-engine)
    * [LSP](/wasplsp/README.md#language-server-protocol)
    * [Utils](/wasputils/README.md#command-line-utilities)
2. [Getting Started](#getting-started) - code compilation requirements and instructions
    * [Requirements](#requirements)
    * [Code Configuration and Compilation](#code-configuration-and-compilation)
3. [Core](/waspcore/README.md#core-package) - core datastructures
    * [String Pool](/waspcore/README.md#string-pool)
    * [Token Pool](/waspcore/README.md#token-pool)
    * [Tree Node Pool](/waspcore/README.md#tree-node-pool)
    * [Interpreter](/waspcore/README.md#interpreter)
4. [Expr](/waspexpr/README.md#expression-engine) - expression engine
    * [Arithmetic and Algabraic Operators](/waspexpr/README.md#arithmetic-and-algabraic-operators)
    * [Relational Operators](/waspexpr/README.md#relational-operators)
    * [Default Variables](/waspexpr/README.md#default-variables)
    * [Default Functions](/waspexpr/README.md#default-functions)
    * [Special Functions](/waspexpr/README.md#special-functions)
    * [Array Access](/waspexpr/README.md#array-access)
5. [HIVE](/wasphive/README.md#hive) - HierarchicAL Input Validation Engine (HIVE)
    * [Overview](/wasphive/README.md#hive)
    * [Input Validation Rules Summary](/wasphive/README.md#input-validation-rules-summary)
        * [Input Validation Details and Examples](/wasphive/README.md#input-validation-details-and-examples)
        * [Miscellaneous Details and Examples](/wasphive/README.md#miscellaneous-details-and-examples)
        * [MinOccurs Details and Examples](/wasphive/README.md#minoccurs-details-and-examples)
        * [MaxOccurs Details and Examples](/wasphive/README.md#maxoccurs-details-and-examples)
        * [ValType Details and Examples](/wasphive/README.md#valtype-details-and-examples)
        * [ValEnums Details and Examples](/wasphive/README.md#valenums-details-and-examples)
        * [MinValInc Details and Examples](/wasphive/README.md#minvalinc-details-and-examples)
        * [MaxValInc Details and Examples](/wasphive/README.md#maxvalinc-details-and-examples)
        * [MinValExc Details and Examples](/wasphive/README.md#minvalexc-details-and-examples)
        * [MaxValExc Details and Examples](/wasphive/README.md#maxvalexc-details-and-examples)
        * [ExistsIn Details and Examples](/wasphive/README.md#existsin-details-and-examples)
        * [NotExistsIn Details and Examples](/wasphive/README.md#notexistsin-details-and-examples)
        * [SumOver Details and Examples](/wasphive/README.md#sumover-details-and-examples)
        * [SumOverGroup Details and Examples](/wasphive/README.md#sumovergroup-details-and-examples)
        * [IncreaseOver Details and Examples](/wasphive/README.md#increaseover-details-and-examples)
        * [DecreaseOver Details and Examples](/wasphive/README.md#decreaseover-details-and-examples)
        * [ChildAtMostOne Details and Examples](/wasphive/README.md#childatmostone-details-and-examples)
        * [ChildExactlyOne Details and Examples](/wasphive/README.md#childexactlyone-details-and-examples)
        * [ChildAtLeastOne Details and Examples](/wasphive/README.md#childatleastone-details-and-examples)
        * [ChildCountEqual Details and Examples](/wasphive/README.md#childcountequal-details-and-examples)
        * [ChildUniqueness Details and Examples](/wasphive/README.md#childuniqueness-details-and-examples)
    * [Input Assistance Details](/wasphive/README.md#input-assistance-details)
        * [MaxOccurs Assistance Details](/wasphive/README.md#maxoccurs-assistance-details)
        * [ChildAtMostOne Assistance Details](/wasphive/README.md#childatmostone-assistance-details)
        * [ChildExactlyOne Assistance Details](/wasphive/README.md#childexactlyone-assistance-details)
        * [ValEnums Assistance Details](/wasphive/README.md#valenums-assistance-details)
        * [ValType Assistance Details](/wasphive/README.md#valtype-assistance-details)
        * [ExistsIn Assistance Details](/wasphive/README.md#existsin-assistance-details)
        * [InputTmpl Assistance Details](/wasphive/README.md#inputtmpl-assistance-details)
        * [InputName Assistance Details](/wasphive/README.md#inputname-assistance-details)
        * [InputType Assistance Details](/wasphive/README.md#inputtype-assistance-details)
        * [InputVariants Assistance Details](/wasphive/README.md#inputvariants-assistance-details)
        * [InputDefault Assistance Details](/wasphive/README.md#inputdefault-assistance-details)
        * [Description Assistance Details](/wasphive/README.md#description-assistance-details) 
6. [SIREN](/waspsiren/README.md#sequence-input-retrieval-engine-siren) - Simple Input Retrieval ENgine (SIREN) 
    * [Selecting Nodes](/waspsiren/README.md#selecting-nodes)
    * [Selection Predicates](/waspsiren/README.md#predicates)
    * [Selecting Unknown Nodes](/waspsiren/README.md#selecting-unknown-nodes)
7. [SON](/waspson/README.md#standard-object-notation-son) - Standard Object Notation (SON) for general input data entry
8. [DDI](/waspddi/README.md#definition-driven-interpreter) - Definition Driven Interpreter (DDI) for reduced input data entry  
9. [EDDI](/waspeddi/README.md#eddi-input-interpreter) - Extra Definition Driven Interpreter (EDDI) for reduced input data entry  
10. [HALITE](/wasphalite/README.md#halite-engine) - HierarchicAL Input Template Engine (HALITE) 
    * [Template Evaluation Summary](/wasphalite/README.md#template-evaluation-summary)
    * [Attributes and Expressions](/wasphalite/README.md#attributes-and-expressions)
        * [Intrinsic Attributes](/wasphalite/README.md#intrinsic-attributes)  
        * [Silent Attributes](/wasphalite/README.md#silent-attributes)
        * [Optional Attributes](/wasphalite/README.md#optional-attributes)
        * [Attribute Patterns](/wasphalite/README.md#attribute-patterns)
        * [Expressions](/wasphalite/README.md#expressions)
        * [Formatting](/wasphalite/README.md#formatting)
            * [Format Flags](/wasphalite/README.md#format-flags)
            * [Format Width](/wasphalite/README.md#format-width)
            * [Format Precision](/wasphalite/README.md#format-precision)
    * [Scoped Attribute](/wasphalite/README.md#scoped-attribute)
        * [Object Scoped Attribute](/wasphalite/README.md#object-scoped-attribute)
        * [Array Scoped Attribute](/wasphalite/README.md#object-scoped-attribute)
    * [File Imports](/wasphalite/README.md#file-imports)
        * [Parameterized File Imports](/wasphalite/README.md#parameterized-file-import)
            * [File Import using an Object](/wasphalite/README.md#file-import-using-an-object)
            * [Iterative File Import using an Array](/wasphalite/README.md#iterative-file-import-using-an-array-or-ranges)
    * [Conditional Blocks](/wasphalite/README.md#conditional-blocks)
11. [INI](/waspson/README.md#standard-object-notation-son) - configuration file format 
12. [LSP](/wasplsp/README.md#language-server-protocol) - Language Server Protocol support
    * [WASP Language Server](/wasplsp/README.md#wasp-language-server)
    * [Client](/wasplsp/README.md#client)
    * [LSPInterpreter](/wasplsp/README.md#lspinterpreter)
13. [Utils](/wasputils/README.md#command-line-utilities) - utilities for command line file interaction (listing, validating, selecting, etc.)
    * [File Listing Utilities](/wasputils/README.md#file-listing-utilities)
    * [File Component Selection Utilities](/wasputils/README.md#file-component-selection-utilities)
    * [XML Utilities](/wasputils/README.md#xml-utilities)
    * [File Validation](/wasputils/README.md#file-validation-utilities)
    * [SON/DDI to JSON Utilities](/wasputils/README.md#sonddi-to-json-utilities)
    * [HALITE](/wasputils/README.md#the-hierarchal-input-template-expansion-engine)
    * [Schema Skeleton Creation Utility](/wasputils/README.md#schema-skeleton-creation-utility)
    * [Autogenerate Input Documentation](/wasputils/README.md#autogenerate-input-documentation)


# Components
WASP is composed of the following primary components:

1. [__Core__](/waspcore/README.md#core-package): the waspcore package contains most necessary data structures and interface classes needed to interact with text files. 
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
2. [__Expr__](/waspexpr/README.md#expression-engine): the waspexpr package contains lexer, parser, and evaluation logic for mathematical expressions.
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
4. [__HIVE__](/wasphive/README.md#hive): the Hierarchical Input Validation Engine contains algorithms for validating a parse tree using a document schema/definition file, the flexible scalar and referential rules - supporting 
    * element occurrence,
    * value,
    * child uniqueness and choice,
    * existence,
    * sum,
    * predicated sum, etc.  
5. [__JSON__](http://www.json.org/): the waspjson package contains a lexer, as well as a parser for the JSON grammar (http://www.json.org/).
6. [__SIREN__](/waspsiren/README.md#sequence-input-retrieval-engine-siren): the Sequence Input Retrieval Engine (SIREN) contains a lexer, parser, and evaluation logic for tree node lookup; the flexible tree node lookup mechanism supports
    * absolute and relative wild-carded names and value, or
    * index-predicated node path lookup
7. [__SON__](/waspson/README.md#standard-object-notation-son): the Standard Object Notation (SON) - waspson package contains the lexer, parser, and tree node view for the SON grammar; the flexible, structured, input entry mechanism supports
    * blocks, objects, arrays, and keyed values, as well as
    * identified objects, arrays, and keyed values.
8. [__DDI__](waspddi/README.md#definition-driven-interpreter): the Definition-Driven Interpreter (DDI) contains a lexer, a parser, and an interpreter for the lightweight input format. 
    * Hierarchical Input format with very little syntax.
9. [__EDDI__](waspeddi/README.md#eddi-input-interpreter): the Extra Definition Driven Interpreter (EDDI) contains a lexer, a parser, and an interpreter for the lightweight input format. 
    * Hierarchical Input format with little syntax representing blocks, lists, sections, and scalar data.
    * Allows input field annotation which greatly enhances input validation capabilities 
10. [__HALITE__](/wasphalite/README.md#halite-engine): the Hierarchical Input Template Expansion engine provides a data-driven means of expanding patterned input.
    * Supports attribute and expression evaluations.
    * Supports template imports.
    * Supports conditional action blocks.
11. [__INI__](/waspson/README.md#standard-object-notation-son): configuration file format, supported by SON.
12. [__LSP__](/wasplsp/README.md#language-server-protocol): Language Server Protocol support
13. [__Utils__](/wasputils/README.md#command-line-utilities) - the wasputils package contains executable utilities for listing/viewing, selecting, validating, and transforming WASP-supported grammars.
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

