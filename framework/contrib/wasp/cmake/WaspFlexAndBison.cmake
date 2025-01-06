##---------------------------------------------------------------------------##
## WaspFlexAndBison.cmake
## Jordan P. Lefebvre, lefebvrejp@ornl.gov
##---------------------------------------------------------------------------##
FUNCTION(WASP_FLEX_AND_BISON)
  SET(oneValueArgs FLEX_FILE BISON_FILE)
  SET(multiValueArgs BISON_OPTIONS FLEX_OPTIONS SED_OPTIONS)
  CMAKE_PARSE_ARGUMENTS(PARSE "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )
  IF(NOT PARSE_FLEX_FILE)
    MESSAGE(FATAL_ERROR "WASP_FLEX_AND_BISON missing FLEX_FILE")
  ENDIF()
  IF(NOT PARSE_BISON_FILE)
    MESSAGE(FATAL_ERROR "WASP_FLEX_AND_BISON missing BISON_FILE")
  ENDIF()
  IF(NOT FLEX_PATH)
    FIND_PROGRAM(FLEX_PATH NAME flex HINTS ENV PATH)
    IF(NOT FLEX_PATH STREQUAL "FLEX_PATH-NOTFOUND")
      SET(FLEX_PATH ${FLEX_PATH} CACHE STRING "Path to flex executable.")
      MESSAGE(STATUS "Found flex : '${FLEX_PATH}'")
    ENDIF()
  ENDIF()
  IF(NOT SED_EXE_PATH)
    FIND_PROGRAM(SED_EXE_PATH NAME sed HINTS ENV PATH)
    IF(NOT SED_EXE_PATH STREQUAL "SED_EXE_PATH-NOTFOUND")
      SET(SED_EXE_PATH ${SED_EXE_PATH} CACHE STRING "Path to sed executable.")
      MESSAGE(STATUS "Found sed: '${SED_EXE_PATH}'")
    ENDIF()
  ENDIF()
  IF(NOT BISON_PATH)
    FIND_PROGRAM(BISON_PATH NAME bison HINTS ENV PATH)
    IF(NOT BISON_PATH STREQUAL "BISON_PATH-NOTFOUND")
      SET(BISON_PATH ${BISON_PATH} CACHE STRING "Path to bison executable.")
      MESSAGE(STATUS "Found bison : '${BISON_PATH}'")
    ENDIF()
  ENDIF()

  IF(FLEX_PATH STREQUAL "FLEX_PATH-NOTFOUND")
     MESSAGE("--NOTICE: Flex not found. Generating/updating lexer disabled.")
  ELSE()
     GET_FILENAME_COMPONENT(lex_name ${PARSE_FLEX_FILE} NAME)
     SET(lex_target_name flex_${lex_name})
     MESSAGE(STATUS "Adding flex/bison target: ${lex_target_name}")
     ADD_CUSTOM_TARGET(${lex_target_name}
         COMMAND ${FLEX_PATH} ${PARSE_FLEX_OPTIONS} ${PARSE_FLEX_FILE}
         WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
         DEPENDS "${PARSE_FLEX_FILE}")
    IF(PARSE_SED_OPTIONS)
      IF(SED_EXE_PATH STREQUAL "SED_EXE_PATH-NOTFOUND")
         MESSAGE("--NOTICE: sed not found. Fixing lexer include disabled.")
      ELSE()
         SET(sed_target_name sed_${lex_name})
         MESSAGE(STATUS "Adding flex/bison target: ${sed_target_name}")
         ADD_CUSTOM_TARGET(${sed_target_name}
             COMMAND ${SED_EXE_PATH} -i '' ${PARSE_SED_OPTIONS}
             WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
             DEPENDS ${lex_target_name} "${PARSE_FLEX_FILE}"
         )
      ENDIF()
    ENDIF()

    IF(BISON_PATH STREQUAL "BISON_PATH-NOTFOUND")
       MESSAGE("--NOTICE: Bison not found. Generating/updating parser disabled.")
    ELSE()
       GET_FILENAME_COMPONENT(name ${PARSE_BISON_FILE} NAME)
       SET(name bison_${name})
       MESSAGE(STATUS "Adding flex/bison target: ${name}")
       ADD_CUSTOM_TARGET(${name}
           COMMAND ${BISON_PATH} ${PARSE_BISON_FILE} ${PARSE_BISON_OPTIONS}
           WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
           DEPENDS ${sed_target_name} ${lex_target_name} "${PARSE_FLEX_FILE}" "${PARSE_BISON_FILE}")
    ENDIF()
  ENDIF()

ENDFUNCTION()
