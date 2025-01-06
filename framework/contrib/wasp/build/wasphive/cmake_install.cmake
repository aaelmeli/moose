# Install script for directory: /home/elmeabde/sawtooth1/projects/moose/framework/contrib/wasp/wasphive

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/home/elmeabde/sawtooth1/projects/moose/framework/contrib/wasp/install")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "RELEASE")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "0")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/usr/bin/objdump")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libwasphive.so.3.1.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libwasphive.so.03"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHECK
           FILE "${file}"
           RPATH "/home/elmeabde/sawtooth1/projects/moose/framework/contrib/wasp/install/lib:$ORIGIN/../lib")
    endif()
  endforeach()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES
    "/home/elmeabde/sawtooth1/projects/moose/framework/contrib/wasp/build/wasphive/libwasphive.so.3.1.0"
    "/home/elmeabde/sawtooth1/projects/moose/framework/contrib/wasp/build/wasphive/libwasphive.so.03"
    )
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libwasphive.so.3.1.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libwasphive.so.03"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHANGE
           FILE "${file}"
           OLD_RPATH "/home/elmeabde/sawtooth1/projects/moose/framework/contrib/wasp/build/waspsiren:/home/elmeabde/sawtooth1/projects/moose/framework/contrib/wasp/build/waspcore:"
           NEW_RPATH "/home/elmeabde/sawtooth1/projects/moose/framework/contrib/wasp/install/lib:$ORIGIN/../lib")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/usr/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "/home/elmeabde/sawtooth1/projects/moose/framework/contrib/wasp/build/wasphive/libwasphive.so")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/wasphive" TYPE FILE FILES
    "/home/elmeabde/sawtooth1/projects/moose/framework/contrib/wasp/wasphive/AlphaNum.h"
    "/home/elmeabde/sawtooth1/projects/moose/framework/contrib/wasp/wasphive/HIVE.h"
    "/home/elmeabde/sawtooth1/projects/moose/framework/contrib/wasp/wasphive/HIVE.i.h"
    "/home/elmeabde/sawtooth1/projects/moose/framework/contrib/wasp/wasphive/InputDefinition.h"
    "/home/elmeabde/sawtooth1/projects/moose/framework/contrib/wasp/wasphive/InputDefinition.i.h"
    )
endif()

