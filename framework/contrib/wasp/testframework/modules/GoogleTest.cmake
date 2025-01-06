#---------------------------------------------------------------------------##
## modules/GoogleTest.cmake
## Jordan P. Lefebvre
## Thursday March 4 14:35:42 2016
##---------------------------------------------------------------------------##
## Setup google test
##---------------------------------------------------------------------------##

INCLUDE(CMakeParseArguments)
INCLUDE(AssertDefined)

##---------------------------------------------------------------------------##
## ADDING UNIT TEST
##---------------------------------------------------------------------------##
# ADD_GOOGLE_TEST(
#   SOURCE_FILE
#   [TIMEOUT seconds]
#   [NP 1 [2 [...]]]
#   [DEPLIBS lib1 [lib2 ...]]
#   [ENVIRONMENT VAR=value [VAR2=value2 ...]]
#   [ISOLATE]
#   [DISABLE]
#   [QTCOREAPP | QTAPP]
#   )
#
# Create and add a unit test from the source file SOURCE_FILE.
#
# NP specifies the number of processors to use for this unit test. The default
# is to use NemesisNP (1, 2, and 4) for MPI builds and 1 for serial builds.
#
# DEPLIBS specifies extra libraries to link to. By default, unit tests will link
# against the package's current library.
#
# ENVIRONMENT sets the given environmental variables when the test is run.
#
# If ISOLATE is specified, the test will be run in its own directory.
#
# If DISABLE is specified, we will build the test executable but omit it from
# the list of tests to run through CTest.
#
# If QTCOREAPP is specified, we will build the test executable with QtCoreAppFixture
#
# If QTAPP is specified, we will build the test executable with QtAppFixture
#
FUNCTION(ADD_GOOGLE_TEST SOURCE_FILE)
  cmake_parse_arguments(PARSE
    "ISOLATE;DISABLE;QTCOREAPP;QTAPP"
    "TIMEOUT"
    "DEPLIBS;NP;ENVIRONMENT" ${ARGN})


  IF(PARSE_QTAPP)
    SET(MAIN_EXECUTABLE_DEPLIB TESTFRAMEWORK_QAPP_DEPLIB)
  ELSEIF(PARSE_QTCOREAPP)
    SET(MAIN_EXECUTABLE_DEPLIB TESTFRAMEWORK_QCOREAPP_DEPLIB)
  ELSE()
    SET(MAIN_EXECUTABLE_DEPLIB TESTFRAMEWORK_BASE_DEPLIB)
  ENDIF()


  # Add additional library dependencies if needed
  IF(PARSE_DEPLIBS)
    SET(DEPLIBS_PARM TESTONLYLIBS ${PARSE_DEPLIBS} ${MAIN_EXECUTABLE_DEPLIB})
  ELSE()
    SET(DEPLIBS_PARM TESTONLYLIBS ${MAIN_EXECUTABLE_DEPLIB})
  ENDIF()

  # Set number of processors, defaulting to 1
  SET(NUM_PROCS ${PARSE_NP})
  IF (NOT NUM_PROCS)
    SET(NUM_PROCS 1)
  ENDIF()

  # Check to see if MPI-only unit test
  LIST(FIND NUM_PROCS 1 HAS_SERIAL)
  IF (HAS_SERIAL EQUAL -1)
    SET(COMM mpi)
    IF(NOT TPL_ENABLE_MPI)
      # return early to avoid potential set_property on nonexistent test
      RETURN()
    ENDIF()
  ELSE()
    # Tribits COMM specifier
    SET(COMM serial mpi)
  ENDIF()

  # If not using MPI, make sure NP is only 1
  IF (NOT TPL_ENABLE_MPI)
    SET(NUM_PROCS 1)
  ENDIF()

  # add the test executable
  GET_FILENAME_COMPONENT(EXE_NAME ${SOURCE_FILE} NAME_WE)
  TRIBITS_ADD_EXECUTABLE(
    ${EXE_NAME}
    SOURCES ${SOURCE_FILE}
    ${DEPLIBS_PARM}
    COMM ${COMM}
    )

  # If the test is disabled, notify configuration
  IF (PARSE_DISABLE)
    MESSAGE("Disabling testing for ${SOURCE_FILE} in ${PACKAGE_NAME}")
    RETURN()
  ENDIF()

  # Loop over processors for parallel tests
  FOREACH(np ${NUM_PROCS})
    IF (PARSE_ISOLATE)
      IF (TPL_ENABLE_MPI)
        # Add an "advanced" test
        TRIBITS_ADD_ADVANCED_TEST(
          ${EXE_NAME}_MPI_${np}
          TEST_0
            EXEC ${EXE_NAME}
            NUM_MPI_PROCS ${np}
          OVERALL_WORKING_DIRECTORY TEST_NAME
          )
      ELSE()
        # Add an "advanced" test
        TRIBITS_ADD_ADVANCED_TEST(
          ${EXE_NAME}
          TEST_0
            EXEC ${EXE_NAME}
          OVERALL_WORKING_DIRECTORY TEST_NAME
          )
      ENDIF()
    ELSE()
      # Add a normal test
      TRIBITS_ADD_TEST(
        ${EXE_NAME}
        NUM_MPI_PROCS ${np}
        )
    ENDIF()
  ENDFOREACH()

  # set environmental variables if necessary
  IF(PARSE_ENVIRONMENT OR PARSE_TIMEOUT)
    FOREACH(np ${NUM_PROCS})
      IF (TPL_ENABLE_MPI)
        SET(TEST_NAME "${PACKAGE_NAME}_${EXE_NAME}_MPI_${np}")
      ELSE()
        SET(TEST_NAME "${PACKAGE_NAME}_${EXE_NAME}")
      ENDIF()

      # Modify environment
      IF(PARSE_ENVIRONMENT)
        SET_PROPERTY(TEST "${TEST_NAME}"
          PROPERTY ENVIRONMENT ${PARSE_ENVIRONMENT})
      ENDIF()

      IF(PARSE_TIMEOUT)
        SET_PROPERTY(TEST "${TEST_NAME}"
          PROPERTY TIMEOUT ${PARSE_TIMEOUT})
      ENDIF()

    ENDFOREACH()
  ENDIF()

ENDFUNCTION()

##---------------------------------------------------------------------------##
##                   end of GoogleTest.cmake
##---------------------------------------------------------------------------##
