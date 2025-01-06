#
# We need to add the 'modules' directory
# which will make available the ADD_GOOGLE_TEST macro
GET_FILENAME_COMPONENT(THIS_ABSOLUTE_BASE "${CMAKE_CURRENT_SOURCE_DIR}/.." ABSOLUTE)
MESSAGE(STATUS "Setting up testframework...")
MESSAGE(STATUS "Adding ${THIS_ABSOLUTE_BASE}/modules to CMAKE_MODULE_PATH")
SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${THIS_ABSOLUTE_BASE}/modules/" PARENT_SCOPE)

#
# Set cache variables for include files
SET(TESTFRAMEWORK_BASE_MAIN_
   "${THIS_ABSOLUTE_BASE}/gtest_main.cc"
   PARENT_SCOPE)
SET(TESTFRAMEWORK_CORE_MAIN_
   "${THIS_ABSOLUTE_BASE}/gtest_qcoreapp_main.cc"
   PARENT_SCOPE)
SET(TESTFRAMEWORK_QAPP_MAIN_
   "${THIS_ABSOLUTE_BASE}/gtest_qapp_main.cc"
    PARENT_SCOPE)

