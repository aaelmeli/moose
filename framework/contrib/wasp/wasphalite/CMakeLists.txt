##---------------------------------------------------------------------------##
## HeirarchicAL Input Template Expansion (HALITE)/CMakeLists.txt
## Robert A. Lefebvre
## Friday 03/17/2016
##---------------------------------------------------------------------------##
## Copyright (C) 2017 Oak Ridge National Laboratory, UT-Battelle, LLC.
##---------------------------------------------------------------------------##
## CMAKE for HALITE
##---------------------------------------------------------------------------##

INCLUDE(WaspAddCompilerFlags)


TRIBITS_SUBPACKAGE(halite)
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
DataAccessor.cpp
SubStringIndexer.cpp
HaliteWorkflow.cpp
HaliteNodeView.cpp
)

##---------------------------------------------------------------------------##
## PACKAGE TARGETS AND INSTALL
##---------------------------------------------------------------------------##

TRIBITS_ADD_LIBRARY(
  wasphalite
  SOURCES ${SOURCES}
  NOINSTALLHEADERS ${HEADERS})

INSTALL(FILES
  ${HEADERS}
  DESTINATION
  include/wasphalite)

#
# Add test directory for this package
#
TRIBITS_ADD_TEST_DIRECTORIES(test)

##---------------------------------------------------------------------------##
##                   end of CMakeLists.txt
##---------------------------------------------------------------------------##
TRIBITS_SUBPACKAGE_POSTPROCESS()
