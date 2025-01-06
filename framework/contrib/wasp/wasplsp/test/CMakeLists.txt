INCLUDE(GoogleTest)

SET_DEFAULT(LSP_TEST_DIR "${CMAKE_CURRENT_SOURCE_DIR}" )
configure_file("Paths.h.in" "${CMAKE_CURRENT_BINARY_DIR}/Paths.h" @ONLY)

TRIBITS_ADD_LIBRARY(test_server_lib
  SOURCES TestServer.cpp TestServer.h
  DEPLIBS wasplsp
  TESTONLY
)

ADD_GOOGLE_TEST(tstLSP.cpp              NP 1)
ADD_GOOGLE_TEST(tstServer.cpp           NP 1 DEPLIBS test_server_lib)
ADD_GOOGLE_TEST(tstIntegrate.cpp        NP 1 DEPLIBS test_server_lib)
ADD_GOOGLE_TEST(tstClient.cpp           NP 1 DEPLIBS test_server_lib)
ADD_GOOGLE_TEST(tstWaspClientServer.cpp NP 1)
ADD_GOOGLE_TEST(tstLSPInterpreter.cpp   NP 1 DEPLIBS test_server_lib)
