##---------------------------------------------------------------------------##
## testframework/CMakeLists.txt
## Jordan Lefebvre
## Monday March 7 10:19:18 2016
##---------------------------------------------------------------------------##


#
# Project designed around header only test classes
#
TRIBITS_PACKAGE(testframework)

#
# Integrate with the vanilla google test cmake
IF(NOT googletest-distribution_BINARY_DIR)
    MESSAGE(FATAL "googletest is required to be processed prior to testframework"
	    "Please add_subdirectory(googletest).")
ELSE()
    # Add include directory for 'gtest/gtest/h' include.
    SET(gtest_include_dir "${googletest-distribution_SOURCE_DIR}/googletest/include/")
    MESSAGE(STATUS "Adding googletest source dir: ${gtest_include_dir}")
    INCLUDE_DIRECTORIES(${gtest_include_dir})
ENDIF()
#
# Add different front-end executables as test only libraries
TRIBITS_ADD_LIBRARY(TESTFRAMEWORK_BASE_DEPLIB
  SOURCES gtest_main.cc
  TESTONLY
)
TARGET_LINK_LIBRARIES(TESTFRAMEWORK_BASE_DEPLIB gtest)
#
# Add different front-end executables that depend on Qt as test only libraries
IF(TPL_ENABLE_Qt5Core OR TPL_ENABLE_QT)
  TRIBITS_ADD_LIBRARY(TESTFRAMEWORK_QAPP_DEPLIB
    SOURCES gtest_qapp_main.cc
    TESTONLY
  )
  TARGET_LINK_LIBRARIES(TESTFRAMEWORK_QAPP_DEPLIB gtest)
  TRIBITS_ADD_LIBRARY(TESTFRAMEWORK_QCOREAPP_DEPLIB
    SOURCES gtest_qcoreapp_main.cc
    TESTONLY
  )
  TARGET_LINK_LIBRARIES(TESTFRAMEWORK_QCOREAPP_DEPLIB gtest)
ENDIF()

TRIBITS_PACKAGE_POSTPROCESS()
