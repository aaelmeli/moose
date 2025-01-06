##---------------------------------------------------------------------------##
## wasp/Version.cmake
##---------------------------------------------------------------------------##
# Single file that needs to be changed on a release branch
# or on the development branch in order to configure wasp
# for release mode and set the version.

SET(${PROJECT_NAME}_NAME "wasp")
SET(${PROJECT_NAME}_VERSION 3.1.0)
SET(${PROJECT_NAME}_FULL_VERSION "Workbench Analysis Sequence Processor 3.1.0")
SET(${PROJECT_NAME}_VERSION_STRING "3.1.0")
SET(${PROJECT_NAME}_MAJOR_VERSION 03)
SET(${PROJECT_NAME}_MAJOR_MINOR_VERSION 030100)
SET(${PROJECT_NAME}_ENABLE_DEVELOPMENT_MODE_DEFAULT ON) # Change to 'OFF' for a release (beta or final)

##---------------------------------------------------------------------------##
##                    end of wasp/Version.cmake
##---------------------------------------------------------------------------##
