##---------------------------------------------------------------------------##
## WaspAddCompilerFlag.cmake
## Jordan P. Lefebvre, lefebvrejp@ornl.gov
##---------------------------------------------------------------------------##



IF( CMAKE_CXX_COMPILER )
   INCLUDE(CheckCXXCompilerFlag)
   #
   # See if the compiler allows the given flags; add them to CXX flags.
   MACRO(WASP_ADD_CXX_FLAGS)
     FOREACH(_FLAG ${ARGN})
       STRING(REGEX REPLACE "[^0-9a-zA-Z]" "_" FLAGNAME ${_FLAG})
       CHECK_CXX_COMPILER_FLAG("${_FLAG}" WASP_USE_CXX_FLAG_${FLAGNAME})
       IF(WASP_USE_CXX_FLAG_${FLAGNAME})
         SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${_FLAG}")
       ENDIF()
     ENDFOREACH()
   ENDMACRO()
ENDIF()

IF( CMAKE_C_COMPILER )
   INCLUDE(CheckCCompilerFlag)
   #
   # See if the compiler allows the given flags; add them to C flags.
   MACRO(WASP_ADD_C_FLAGS)
     FOREACH(_FLAG ${ARGN})
       STRING(REGEX REPLACE "[^0-9a-zA-Z]" "_" FLAGNAME ${_FLAG})
       CHECK_C_COMPILER_FLAG("${_FLAG}" WASP_USE_C_FLAG_${FLAGNAME})
       IF(WASP_USE_C_FLAG_${FLAGNAME})
         SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${_FLAG}")
       ENDIF()
     ENDFOREACH()
ENDMACRO()

ENDIF()

# - Check if given Fortran source compiles and links into an executable
# CHECK_Fortran_SOURCE_COMPILES(<code> <var> [FAIL_REGEX <fail-regex>])
#  <code>       - source code to try to compile, must define 'program'
#  <var>        - variable to store whether the source code compiled
#  <fail-regex> - fail if test output matches this regex
# The following variables may be set before calling this macro to
# modify the way the check is run:
#
#  CMAKE_REQUIRED_FLAGS = string of compile command line flags
#  CMAKE_REQUIRED_DEFINITIONS = list of macros to define (-DFOO=bar)
#  CMAKE_REQUIRED_INCLUDES = list of include directories
#  CMAKE_REQUIRED_LIBRARIES = list of libraries to link


MACRO(CHECK_Fortran_SOURCE_COMPILES SOURCE VAR)
    SET(_FAIL_REGEX)
    SET(_key)
    FOREACH(arg ${ARGN})
      IF("${arg}" MATCHES "^(FAIL_REGEX)$")
        SET(_key "${arg}")
      ELSEIF(_key)
        LIST(APPEND _${_key} "${arg}")
      ELSE()
        MESSAGE(FATAL_ERROR "Unknown argument:\n  ${arg}\n")
      ENDIF()
    ENDFOREACH()

    SET(MACRO_CHECK_FUNCTION_DEFINITIONS
      "-D${VAR} ${CMAKE_REQUIRED_FLAGS}")
    IF(CMAKE_REQUIRED_LIBRARIES)
      SET(CHECK_Fortran_SOURCE_COMPILES_ADD_LIBRARIES
        "-DLINK_LIBRARIES:STRING=${CMAKE_REQUIRED_LIBRARIES}")
    ELSE()
      SET(CHECK_Fortran_SOURCE_COMPILES_ADD_LIBRARIES)
    ENDIF()
    IF(CMAKE_REQUIRED_INCLUDES)
      SET(CHECK_Fortran_SOURCE_COMPILES_ADD_INCLUDES
        "-DINCLUDE_DIRECTORIES:STRING=${CMAKE_REQUIRED_INCLUDES}")
    ELSE()
      SET(CHECK_Fortran_SOURCE_COMPILES_ADD_INCLUDES)
    ENDIF()
    FILE(WRITE "${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeTmp/src.f90"
      "${SOURCE}\n")

    TRY_COMPILE(${VAR}
      ${CMAKE_BINARY_DIR}
      ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeTmp/src.f90
      COMPILE_DEFINITIONS ${CMAKE_REQUIRED_DEFINITIONS}
      CMAKE_FLAGS -DCOMPILE_DEFINITIONS:STRING=${MACRO_CHECK_FUNCTION_DEFINITIONS}
      "${CHECK_Fortran_SOURCE_COMPILES_ADD_LIBRARIES}"
      "${CHECK_Fortran_SOURCE_COMPILES_ADD_INCLUDES}"
      OUTPUT_VARIABLE OUTPUT)

    FOREACH(_regex ${_FAIL_REGEX})
      IF("${OUTPUT}" MATCHES "${_regex}")
        SET(${VAR} 0)
      ENDIF()
    ENDFOREACH()
ENDMACRO()

# - Check whether the Fortran compiler supports a given flag.
# CHECK_Fortran_COMPILER_FLAG(<flag> <var>)
#  <flag> - the compiler flag
#  <var>  - variable to store the result
# This internally calls the check_fortran_source_compiles macro.  See help
# for CheckFortranSourceCompiles for a listing of variables that can
# modify the build.

MACRO (CHECK_Fortran_COMPILER_FLAG _FLAG _RESULT)
   SET(SAFE_CMAKE_REQUIRED_DEFINITIONS "${CMAKE_REQUIRED_DEFINITIONS}")
   SET(CMAKE_REQUIRED_DEFINITIONS "${_FLAG}")
   CHECK_Fortran_SOURCE_COMPILES("
     program fortran90
          print *, \"Hello World\"
     end program fortran90
     " ${_RESULT}
     # Some compilers do not fail with a bad flag
     FAIL_REGEX "unrecognized .*option"                     # GNU
     FAIL_REGEX "ignoring unknown option"                   # MSVC
     FAIL_REGEX "warning D9002"                             # MSVC, any lang
     FAIL_REGEX "[Uu]nknown option"                         # HP
     FAIL_REGEX "[Ww]arning: [Oo]ption"                     # SunPro
     FAIL_REGEX "command option .* is not recognized"       # XL
     )
   SET (CMAKE_REQUIRED_DEFINITIONS "${SAFE_CMAKE_REQUIRED_DEFINITIONS}")
ENDMACRO()

MACRO(WASP_ADD_Fortran_FLAGS)
  FOREACH(_FLAG ${ARGN})
    STRING(REGEX REPLACE "[^0-9a-zA-Z]" "_" FLAGNAME ${_FLAG})
    CHECK_Fortran_COMPILER_FLAG("${_FLAG}" WASP_USE_Fortran_FLAG_${FLAGNAME})
    IF(WASP_USE_Fortran_FLAG_${FLAGNAME})
      SET(CMAKE_Fortran_FLAGS "${CMAKE_Fortran_FLAGS} ${_FLAG}")
    ENDIF()
  ENDFOREACH()
ENDMACRO()
##---------------------------------------------------------------------------##
##                       end of WaspAddCompilerFlag.cmake
##---------------------------------------------------------------------------##
