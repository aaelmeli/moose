TRIBITS_SUBPACKAGE(py)

file(GLOB MODULE  ${CMAKE_CURRENT_SOURCE_DIR}/*.py )

# Copy module and test directory into build tree for python testing
file(COPY ${MODULE} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY "test"  DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY "drive"  DESTINATION ${CMAKE_CURRENT_BINARY_DIR})


IF(PYTHON_EXECUTABLE)
  # Must use absolute path or Python __file__ test directives are incorrect
  ADD_TEST( tstWaspPy_test_Xml2Obj python ${CMAKE_CURRENT_BINARY_DIR}/test/test_Xml2Obj.py )
  ADD_TEST( tstWaspPy_test_Json2Python python ${CMAKE_CURRENT_BINARY_DIR}/test/test_Json2Python.py  )
  ADD_TEST( tstWaspPy_test_DDI2Python python ${CMAKE_CURRENT_BINARY_DIR}/test/test_DDI2Python.py  )
  ADD_TEST( tstWaspPy_test_Drive python ${CMAKE_CURRENT_BINARY_DIR}/test/test_Drive.py  )
  IF (CMAKE_SYSTEM_NAME STREQUAL  "Linux")
    ADD_TEST( tstWaspPy_test_Drive_Scheduled python ${CMAKE_CURRENT_BINARY_DIR}/test/test_Drive_Scheduled.py  )
  ENDIF()
ENDIF()

#
# Add test directory
TRIBITS_ADD_TEST_DIRECTORIES(test)

# Install the python module
install(DIRECTORY .
        DESTINATION "wasppy"
        PATTERN "./*.py"
)
TRIBITS_SUBPACKAGE_POSTPROCESS()
