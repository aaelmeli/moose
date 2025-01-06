# Changelog
All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](http://keepachangelog.com/en/1.0.0/)
and this project adheres to [Semantic Versioning](http://semver.org/spec/v2.0.0.html).


## [3.1.0] - 5-01-2023

### Added
- wasp2p.get_xml method for converting definition-less input to XML (excludes decorative nodes)

### Changed
- TokenPool::push method now has a `track_newline` parameter, defaulted to `true`
- NodeView/TreeNodePool set_data logic to no longer track newlines. Note added indicating formatting is not preserved once set_data is used

### Fixed
 - Logic for is_null in waspcore generic NodeView
 - HIT parameter values to allow containing ampersand
 - HIT block names to allow starting with a period
 - Issue where prior NodeView::set_data invocations containing newlines caused subsequent NodeView/Interperter::data() calls that reconstructed string using `print_from` to create a padding string involving a string size underflowed to a size too large causing an exception to be thrown


## [3.0.2] - 11-21-2022

### Fixed
 - Interpreter root and node_at to return a null view for no nodes
 - WaspHit lexer for exclusive start condition newline capturing
 - WaspJson lexer for escaped backslash before last double quote
 - WaspServer capabilities to include full-text document syncing

## [3.0.1] - 9-27-2022

### Fixed
- wasphit inability to use tabs as array value separators rather than spaces

## [3.0.0] - 9-15-2022
### Changed
 - Updated NodeView constructor arguments to no longer reference a const interpreter - breaks backward compatibility

### Added
 - Various HIT updates to support integration directly into the MOOSE Framework replacing the existing parser
 - NodeView::set_data method to allow modification of leaf node data in HIT, DDI, EDDI, Halite, JSON, and SON
 - DISABLE_HIT_TYPE_PROMOTION configuration option that lets MOOSE turn off WASP's object name override logic

### Fixed
 - Error in HIT parser logic that could cause an infinite loop when shorthand names are exploded into a hierarchy

## [2.1.0] - 04-22-2022

### Added
 - HIT support for `!include filepath`
 - SON support for `import("filepath")
 - Approximately 30% memory performance improvement in TreeNodePool
 - NodeView Iterator class to enable SIREN/HIVE iterations involving nested interpreters (E.g., included/imported files)

## [2.0.3] - 03-24-2022

### Fixed
 - Missing support for lookup values in HIVE ChildCountEqual validation rule

## [2.0.2] - 03-01-2022

### Fixed
 - Missing installation of waspcore/location.hh
 - Remove the requirement of the 'code' parameter in LSP diagnostic processing because it should be optional

## [2.0.1] - 10-12-2021

### Fixed
 - Inheritance of template delimiters by imported sub-templates

## [2.0.0] - 8-12-2021
### Changed
 - Gtest dependence from TriBITS to the vanilla version
 - Extra repository logic from TriBITS to Git submodule
 - FlexLexer version from 2.5.37 to 2.6.4
 - GNU BISON version from 3.0.4 to 3.7.6
 - WASP Node type of INT to INTEGER (potentially break compatibility with 1.x if you using wasp::INT)
 - Refactored VII into a more generic Extra Definition Driven Interpreter (EDDI) - breaks backward compatibility
 - EDDI input parser to cache staged non-decorative nodes to avoid expensive node-count loop
 - EDDI input Lexer to be 8-bit to support identifying Unicode characters
 - Refacted GetPot into the latest MOOSE-revised Hierarchical Input Text (HIT) - breaks backward compatibility
 - HIT block and subblock to be equivalent
 - HIT comma is no longer a valid array separator
 - HIT semicolon is now an allowed array separator

### Added
 - waspdrive scheduler capability.
 - support for HIT shorthand object notation '[x/y/z]'
 - support for commas in HIT strings
 - support for double-quoted strings in HIT documents
 - support for more than one input and template file in waspdrive input
 - support for EDDI input section naming and validation
 - support for parsing EDDI input list syntax `<n..m x i> 193*10` etc.
 - support for parsing EDDI include files via a list of search locations
 - support for EDDI strings that contain '+' and comments containing unicode characters
 - InputAliases STRIDE construct to support strided aliases as dictated by EDDI input


### Fixed
- wasplsp compile error in certain configurations caused by missing include of `functional`.


## [1.0.3] - 10-07-2019
### Added
- GCC 5.4 compiler compatibility CI job.
- wasplsp - Language Server Protocol package with WaspServer and LSPInterpreter. See WASPLSP readme for details.
- waspdakota: a package for the Dakota UQ coupling. Specifically, support for the Dakota Parameter file format
- wasputils/substitute: a utility that allows direct use of the Dakota parameter file for template attribute substitution
- HIVE UNKNOWN input node validation capability (See HIVE readme for details).

### Changed
- waspeddi hanging comment (comment 1 or more lines below the last parameter) logic to reparent to the block

### Fixed
- wasppy inability to handle file paths with whitespaces.
- CMake package logic to allow for incorporation into a Fortran project.
- Missing support for the apostrophe in XML element name affecting the getpotxml utility.


## [1.0.2] - 07-16-2018
### Added
- DocPrint utility that consumes an input schema and produces GitLab-compatible Markdown. The Markdown can be converted to HTML using Pandoc.

## [1.0.1] - 07-05-2018
### Added
- wasppy/driver capability that allows execution of an external application and results extraction.

## [1.0.0] - 05-11-2018
### Added
- Typedefs for Default TokenPool,TreeNodePool, and Interpreters.
- Hierarchical data attribute references in HALITE. I.e., `<object.key.value>`

### Changed
- The TreeNodeView has been refactored to be NodeView and free of template parameters. This breaks backward compatibility

### Fixed
- inoperable wasp2py when using wasp2py module from a regular wasp installation (i.e., wasp/wasppy/son2py and wasp/bin)
- missing support for `%` in expression engine strings
- missing support for `+` in ddi integer token lex pattern

## [0.1.2] - 01-22-2018
### Added
- The SON syntax now supports a 'block', ([blockname]) syntax which acts the same
as a SON object, but can be visually more appealing to users. It also allows for
compatibility with INI/Config and the NEK5000 PAR input formats.
- A clang-format style standard and updated code accordingly
- A SON2JSON routine and utility that allows conversion and validation
of SON input to JSON. This allows for validated input use in Python.
- HALITE attribute delimiter intrinsic variables for use in rendering
the literal delimiter.

### Fixed
- MVSC 2015 and Intel compile error involving _set_output_format in waspcore/Format.h


## [0.1.1] - 10-22-2017
### Added
- HALITE configurable iterative delimiter emission capability that
allows a delimiter to be emitted based on a given iteration stride.
- HALITEWorkflow convenience class.
- Support for shared library builds on Windows
- Substantial improvements in README documentation
- Missing min/max functions to Expr/HALITE packages

### Changed
- HIVE ChildUniqueness and NotExistsIn to be case-insensitive

### Fixed
- An issue where HALITE indirect attribute substitution produced a parse
error.
- A compiler error when compiling in a Qt project the 'emit' macro and
the HALITE emit method collided.
- An issue where Expr lexer's REAL token regex consumed part of
the alternative boolean operator (7.gt.8=> 7.ERROR, instead of 7.gt.8=> 7 > 8)
- A segfault when HALITE processes an empty attribute


## [0.1.0] - 06-02-2017
### Added
- SON input format support
- DDI input format support
- JSON data format support
- Expr math expression evaluation capability
- HIVE validation engine capability
- HALITE template engine capability
- GetPot input format
- utilities for listing input formats
- utilities for validating input data
- utilities for input format conversion

