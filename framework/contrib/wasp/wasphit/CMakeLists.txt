INCLUDE(WaspAddCompilerFlags)
INCLUDE(WaspFlexAndBison)

TRIBITS_SUBPACKAGE(hit)

WASP_ADD_CXX_FLAGS(
  "-Wno-deprecated-register"
  "-wd4018"
)

WASP_FLEX_AND_BISON(
  FLEX_FILE HIT.lex
  FLEX_OPTIONS -Ca -v
  SED_OPTIONS 's@\#include <FlexLexer.h>@\#include \"waspcore/FlexLexer.h\"@' HITLexer.cpp
  BISON_FILE HIT.bison
  BISON_OPTIONS --report=state --report=lookahead
)

#
# Configure include file that defines the parser type promotion toggle
#
configure_file("HITConfig.h.in" "HITConfig.h")

SET(SOURCE
HITLexer.cpp
HITParser.cpp
HITNodeView.cpp
)

SET(HEADERS
HITLexer.h
HITParser.hpp
HITInterpreter.h
HITInterpreter.i.h
HITNodeView.h
HITNodeView.i.h
HIT.lex
HIT.bison
)

#
# Add library
TRIBITS_ADD_LIBRARY(wasphit
  SOURCES ${SOURCE}
  NOINSTALLHEADERS ${HEADERS}
)

#
# Add binary directory with HITConfig.h to includes for wasphit target
#
target_include_directories(wasphit PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>  
    $<INSTALL_INTERFACE:include/wasphit>
)

#
# Add test directory
TRIBITS_ADD_TEST_DIRECTORIES(test)

INSTALL(FILES ${HEADERS} DESTINATION "include/wasphit/")
TRIBITS_SUBPACKAGE_POSTPROCESS()
