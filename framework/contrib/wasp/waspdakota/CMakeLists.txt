##---------------------------------------------------------------------------##
## WASP Dakota supprt /CMakeLists.txt
## Robert A. Lefebvre
## Tuesday February 19 15:13 2019
## Note: This package captures miscellaneous processing components for the 
## dakota tool (https://dakota.sandia.gov/). 
## The input interpreter is contained in the Definition Driven Interpreter (DDI)
##---------------------------------------------------------------------------##
## Copyright (C) 2019 Oak Ridge National Laboratory, UT-Battelle, LLC.
##---------------------------------------------------------------------------##
## CMAKE for Dakota
##---------------------------------------------------------------------------##

INCLUDE(WaspAddCompilerFlags)
INCLUDE(WaspFlexAndBison)


TRIBITS_SUBPACKAGE(dakota)
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
FILE(GLOB HEADERS *.h *.hh *.hpp *.lex *.bison
    "${CMAKE_CURRENT_BINARY_DIR}/*.h"
    )

# c++ files
SET(SOURCES
ParametersFile.cpp
)

##---------------------------------------------------------------------------##
## PACKAGE TARGETS AND INSTALL
##---------------------------------------------------------------------------##

TRIBITS_ADD_LIBRARY(
  waspdakota
  SOURCES ${SOURCES}
  NOINSTALLHEADERS ${HEADERS})

INSTALL(FILES
  ${HEADERS}
  DESTINATION
  include/waspdakota)

#
# Add test directory for this package
#
TRIBITS_ADD_TEST_DIRECTORIES(test)

##---------------------------------------------------------------------------##
##                   end of Dakota/CMakeLists.txt
##---------------------------------------------------------------------------##
TRIBITS_SUBPACKAGE_POSTPROCESS()
