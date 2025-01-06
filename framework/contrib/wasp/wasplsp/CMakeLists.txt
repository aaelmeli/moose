##---------------------------------------------------------------------------##
## Language Server Protocol (LSP)/CMakeLists.txt
## Brandon Langley
## Wednesday March 25 13:46 2019
##---------------------------------------------------------------------------##
## Copyright (C) 2019 Oak Ridge National Laboratory, UT-Battelle, LLC.
##---------------------------------------------------------------------------##
## CMAKE for LSP
##---------------------------------------------------------------------------##

INCLUDE(WaspAddCompilerFlags)

TRIBITS_SUBPACKAGE(lsp)
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

# C/C++ headers
FILE(GLOB HEADERS *.h *.hh *.hpp
    "${CMAKE_CURRENT_BINARY_DIR}/*.h"
    )

# c++ files
SET(SOURCES
LSP.cpp
ServerImpl.cpp
ClientImpl.cpp
SymbolIterator.cpp
LSPInterpreter.cpp
)

##---------------------------------------------------------------------------##
## PACKAGE TARGETS AND INSTALL
##---------------------------------------------------------------------------##

TRIBITS_ADD_LIBRARY(
  wasplsp
  SOURCES ${SOURCES}
  NOINSTALLHEADERS ${HEADERS})

INSTALL(FILES
  ${HEADERS}
  DESTINATION
  include/wasplsp)

#
# Add test directory for this package
#
TRIBITS_ADD_TEST_DIRECTORIES(test)

##---------------------------------------------------------------------------##
##                   end of LSP/CMakeLists.txt
##---------------------------------------------------------------------------##
TRIBITS_SUBPACKAGE_POSTPROCESS()
