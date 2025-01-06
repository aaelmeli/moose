##---------------------------------------------------------------------------##
## Definition Driven Interpreter (DDI)/CMakeLists.txt
## Robert A. Lefebvre
## Tuesday February 28 20:21 2017
##---------------------------------------------------------------------------##
## Copyright (C) 2017 Oak Ridge National Laboratory, UT-Battelle, LLC.
##---------------------------------------------------------------------------##
## CMAKE for DDI
##---------------------------------------------------------------------------##

INCLUDE(WaspAddCompilerFlags)
INCLUDE(WaspFlexAndBison)


TRIBITS_SUBPACKAGE(ddi)
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
  FLEX_FILE DDILexer.lex
  FLEX_OPTIONS -Ca -v
  SED_OPTIONS 's@\#include <FlexLexer.h>@\#include \"waspcore/FlexLexer.h\"@' DDIParser.cpp
  BISON_FILE DDIParser.bison
  BISON_OPTIONS --report=state --report=lookahead --report=solved -g
)

# C/C++ headers
FILE(GLOB HEADERS *.h *.hh *.hpp *.lex *.bison
    "${CMAKE_CURRENT_BINARY_DIR}/*.h"
    )

# c++ files
SET(SOURCES
DDILexer.cpp
DDIParser.cpp
DDINodeView.cpp
)

##---------------------------------------------------------------------------##
## PACKAGE TARGETS AND INSTALL
##---------------------------------------------------------------------------##

TRIBITS_ADD_LIBRARY(
  waspddi
  SOURCES ${SOURCES}
  NOINSTALLHEADERS ${HEADERS})

INSTALL(FILES
  ${HEADERS}
  DESTINATION
  include/waspddi)

#
# Add test directory for this package
#
TRIBITS_ADD_TEST_DIRECTORIES(test)

##---------------------------------------------------------------------------##
##                   end of DDI/CMakeLists.txt
##---------------------------------------------------------------------------##
TRIBITS_SUBPACKAGE_POSTPROCESS()
