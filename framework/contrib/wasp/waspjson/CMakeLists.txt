##---------------------------------------------------------------------------##
## Java Script Object Notation (JSON)/CMakeLists.txt
## Robert A. Lefebvre
## Thursday January 19 8:03AM 2017
##---------------------------------------------------------------------------##
## Copyright (C) 2017 Oak Ridge National Laboratory, UT-Battelle, LLC.
##---------------------------------------------------------------------------##
## CMAKE for JSON
##---------------------------------------------------------------------------##

INCLUDE(WaspAddCompilerFlags)
INCLUDE(WaspFlexAndBison)


TRIBITS_SUBPACKAGE(json)
SET_DEFAULT(DBC 0)
SET_DEFAULT(TIMING 0)
SET_DEFAULT(DEBUG_OUTPUT 0)
##---------------------------------------------------------------------------##
## INCLUDES

##---------------------------------------------------------------------------##
## HEADER AND SOURCE FILES
##---------------------------------------------------------------------------##

WASP_ADD_CXX_FLAGS(
  "-Wno-deprecated-register"
)

WASP_FLEX_AND_BISON(
  FLEX_FILE JSONLexer.lex
  FLEX_OPTIONS -Ca -v
  SED_OPTIONS 's@\#include <FlexLexer.h>@\#include \"waspcore/FlexLexer.h\"@' JSONParser.cpp
  BISON_FILE JSONParser.bison
  BISON_OPTIONS --report=state --report=lookahead --report=solved -g
)

# C/C++ headers
FILE(GLOB HEADERS *.h *.hh *.hpp *.lex *.bison
    "${CMAKE_CURRENT_BINARY_DIR}/*.h"
    )

# c++ files
SET(SOURCES
JSONLexer.cpp
JSONParser.cpp
JSONNodeView.cpp
JSONObjectLexer.cpp
JSONObjectParser.cpp
)

##---------------------------------------------------------------------------##
## PACKAGE TARGETS AND INSTALL
##---------------------------------------------------------------------------##

TRIBITS_ADD_LIBRARY(
  waspjson
  SOURCES ${SOURCES}
  NOINSTALLHEADERS ${HEADERS})

INSTALL(FILES
  ${HEADERS}
  DESTINATION
  include/waspjson)

#
# Add test directory for this package
#
TRIBITS_ADD_TEST_DIRECTORIES(test)

##---------------------------------------------------------------------------##
##                   end of JSON/CMakeLists.txt
##---------------------------------------------------------------------------##
TRIBITS_SUBPACKAGE_POSTPROCESS()
