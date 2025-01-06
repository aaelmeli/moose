##---------------------------------------------------------------------------##
## WaspDocGen.cmake
## Robert A. Lefebvre, lefebvrera@ornl.gov
## Function to generate docx and pdf from the provided README.md files
## This requires the pandoc utility be installed
##  - Tested on MAC, installed via MacPorts
##    sudo port install pandoc
##    sudo port install texlive-math-science
##    sudo port install texlive-latex-extra
##
## Logic steps are as follows:
## 1. The README.md files will be copied into the build directory.
## 2. The README.md files interdocument links are revised to be local anchor links
##    * This is required because pandoc combines all MD files into a single document
##      prior to final document generation. Unfortunately, it does not do this fixup.
##    * Uses SED in-place find and replace
##    * Requires all link anchors (e.g, #anchor) to be unique
##    * Requires all regular document references ([doc](/somedoc.md) to reference
##      actual anchor within document so as to be properly converted to internal anchor link
## 3. Run pandoc to generate docx for review comments
## 4. Run pandoc to generate pdf for publication/distribution
##---------------------------------------------------------------------------##
FUNCTION(WASP_DOC_GEN)
  MESSAGE(STATUS "Evaluating generation of DOCX and PDF...")
  SET(oneValueArgs DOCX_FILE PDF_FILE)
  SET(multiValueArgs MD_FILES)
  CMAKE_PARSE_ARGUMENTS(PARSE "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )
  IF(NOT PARSE_DOCX_FILE)
    MESSAGE(FATAL_ERROR "WASP_DOC_GEN missing DOCX_FILE")
  ENDIF()
  IF(NOT PARSE_PDF_FILE)
    MESSAGE(FATAL_ERROR "WASP_DOC_GEN missing BISON_FILE")
  ENDIF()

  MESSAGE(STATUS "DOCX : '${PARSE_DOCX_FILE}'")
  MESSAGE(STATUS "PDF : '${PARSE_PDF_FILE}'")

  IF(NOT SED_EXE_PATH)
    FIND_PROGRAM(SED_EXE_PATH NAME sed HINTS ENV PATH)
    IF(NOT SED_EXE_PATH STREQUAL "SED_EXE_PATH-NOTFOUND")
      SET(SED_EXE_PATH ${SED_EXE_PATH} CACHE STRING "Path to sed executable.")
      MESSAGE(STATUS "Found sed: '${SED_EXE_PATH}'")
    ENDIF()
  ENDIF()
  IF(NOT PANDOC_EXE_PATH)
    FIND_PROGRAM(PANDOC_EXE_PATH NAME pandoc HINTS ENV PATH)
    IF(NOT PANDOC_EXE_PATH STREQUAL "PANDOC_EXE_PATH-NOTFOUND")
      SET(PANDOC_EXE_PATH ${PANDOC_EXE_PATH} CACHE STRING "Path to pandoc executable.")
      MESSAGE(STATUS "Found pandoc : '${PANDOC_EXE_PATH}'")
    ENDIF()
  ENDIF()
  # Create make target to copy README.md files to build directory
  SET(CUSTOM_COPY_LOGIC)
  SET(CUSTOM_SED_LOGIC)
  SET(SED_INPLACE_OPTION "") # on Linux sed -i 's/foo/bar/'
  if( ${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Darwin" )
    set(SED_INPLACE_OPTION "''") # on Darwin sed -i '' 's/foo/bar/'
  endif()
  # waspcore/README.md -> buildtree/waspcore/README.md
  FOREACH(md_file ${PARSE_MD_FILES})
    MESSAGE( STATUS "Adding MD copy target for '${md_file}'")
    SET( CUSTOM_COPY_LOGIC ${CUSTOM_COPY_LOGIC}
                     COMMAND ${CMAKE_COMMAND} -E
                         copy \"${wasp_SOURCE_DIR}/${md_file}\" \"${wasp_BINARY_DIR}/${md_file}\"
                         )
    SET( CUSTOM_SED_LOGIC ${CUSTOM_SED_LOGIC}
                     COMMAND ${SED_EXE_PATH} 
                          -i ${SED_INPLACE_OPTION} \"s@/wasp[^\#]*\#\@\#\@g\" \"${wasp_BINARY_DIR}/${md_file}\"
                         )
  ENDFOREACH()
  MESSAGE( STATUS "Transformation copy logic : ${CUSTOM_COPY_LOGIC}" )
  add_custom_target(copy_md 
                     ALL
                     WORKING_DIRECTORY "${wasp_BINARY_DIR}"
                    ${CUSTOM_COPY_LOGIC}
                      )
  add_custom_target(sed_md 
                     WORKING_DIRECTORY "${wasp_BINARY_DIR}"
                    ${CUSTOM_SED_LOGIC}
                    DEPENDS copy_md
                      )
  add_custom_target(doc_docx_gen 
                     WORKING_DIRECTORY "${wasp_BINARY_DIR}"
                     COMMAND ${PANDOC_EXE_PATH} ${PARSE_MD_FILES} 
                     -o ${PARSE_DOCX_FILE} 
                     DEPENDS sed_md
                      )
  add_custom_target(doc_pdf_gen 
                     WORKING_DIRECTORY "${wasp_BINARY_DIR}"
                     COMMAND ${PANDOC_EXE_PATH} ${PARSE_MD_FILES} 
                     -o ${PARSE_PDF_FILE} 
                     DEPENDS sed_md
                      )
  add_custom_target(doc_gen 
                     DEPENDS doc_pdf_gen doc_docx_gen
                      )

  set(WASP_README "WASP_README.md")
  if( "${CMAKE_HOST_SYSTEM_NAME}" STREQUAL "Windows" )
      # Change file separator and add whitespace separator
      string( REPLACE "/README.md" "\\README.md " WIN_PARSE_MD_FILES "${PARSE_MD_FILES}" )
	  MESSAGE(STATUS "The files to concatenate are : ${WIN_PARSE_MD_FILES}")

      add_custom_target(combine_md 
                     ALL
                     COMMENT "Combining ${WIN_PARSE_MD_FILES} into ${WASP_README}"
                     DEPENDS copy_md
                     WORKING_DIRECTORY "${wasp_BINARY_DIR}"
                     COMMAND type ${WIN_PARSE_MD_FILES} > ${WASP_README}					 
                      )
  else()
      add_custom_target(combine_md 
                     ALL
                     COMMENT "Combining ${PARSE_MD_FILES} into ${WASP_README}"
                     DEPENDS sed_md
                     WORKING_DIRECTORY "${wasp_BINARY_DIR}"
                     COMMAND cat ${PARSE_MD_FILES} > ${WASP_README}
                      )
  endif()
  install(FILES "${wasp_BINARY_DIR}/${WASP_README}" DESTINATION .)
 
ENDFUNCTION()
