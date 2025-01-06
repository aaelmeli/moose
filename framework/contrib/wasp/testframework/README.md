# testframework
CMake/TriBITS project integration google test and qt application.

# Integrating into a TriBITS project

1. Add 'testframework' repo to the projects ExtraRepositoriesList.cmake
```
TRIBITS_PROJECT_DEFINE_EXTRA_REPOSITORIES(
  TESTF_REPO "packages/testframework" GIT https:/github.com/lefebvre/testframework "NOPACKAGES" Experimental
)
```
2. Include the 'packages/testframework/setup' into your project's CallbackSetupExtraOptions.cmake MACRO(TRIBITS_REPOSITORY_SETUP_EXTRA_OPTIONS)
```
ADD_SUBDIRECTORY(${${PROJECT_NAME}_SOURCE_DIR}/packages/testframework/setup)
```
3. Include the GoogleTest module in you tests CMakeLists.txt file.
```
INCLUDE(GoogleTest)
```
4. Add a test using the 'ADD_GOOGLE_TEST' macro.
```
ADD_GOOGLE_TEST(tstTest.cc)
```


Please read the [ADD_GOOGLE_TEST documentation](https://github.com/lefebvre/testframework/blob/master/modules/GoogleTest.cmake) for option details.
