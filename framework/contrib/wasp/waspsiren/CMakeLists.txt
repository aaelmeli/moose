INCLUDE(WaspFlexAndBison)
INCLUDE(WaspAddCompilerFlags)

TRIBITS_SUBPACKAGE(siren)

WASP_ADD_CXX_FLAGS(
  "-Wno-deprecated-register"
)

WASP_FLEX_AND_BISON(
  FLEX_FILE SIRENLexer.lex
  FLEX_OPTIONS -Ca -v
  SED_OPTIONS 's@\#include <FlexLexer.h>@\#include \"waspcore/FlexLexer.h\"@' SIRENLexer.cpp
  BISON_FILE SIRENParser.cpp
  BISON_OPTIONS --report=state --report=lookahead --report=solved -g
)

SET(SOURCE
SIRENLexer.cpp
SIRENParser.cpp
)

# C/C++ headers
FILE(GLOB HEADERS *.h *.hh *.hpp *.lex *.bison
    "${CMAKE_CURRENT_BINARY_DIR}/*.h"
    )

#
# Add library
TRIBITS_ADD_LIBRARY(waspsiren
  SOURCES ${SOURCE}
  NOINSTALLHEADERS ${HEADERS}
)

#
# Add test directory
TRIBITS_ADD_TEST_DIRECTORIES(test)

INSTALL(FILES ${HEADERS} DESTINATION "include/waspsiren/")
TRIBITS_SUBPACKAGE_POSTPROCESS()
