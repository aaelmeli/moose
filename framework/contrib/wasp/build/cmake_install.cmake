# Install script for directory: /home/elmeabde/sawtooth1/projects/moose/framework/contrib/wasp

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
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/." TYPE FILE FILES "/home/elmeabde/sawtooth1/projects/moose/framework/contrib/wasp/build/WASP_README.md")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/." TYPE FILE FILES "/home/elmeabde/sawtooth1/projects/moose/framework/contrib/wasp/LICENSE")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/scripts" TYPE PROGRAM FILES "/home/elmeabde/sawtooth1/projects/moose/framework/contrib/wasp/scripts/test_moose_app_integration.sh")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/scripts" TYPE PROGRAM FILES "/home/elmeabde/sawtooth1/projects/moose/framework/contrib/wasp/scripts/test_moose_app_integration.sh")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/home/elmeabde/sawtooth1/projects/moose/framework/contrib/wasp/build/testframework/setup/cmake_install.cmake")
  include("/home/elmeabde/sawtooth1/projects/moose/framework/contrib/wasp/build/waspcore/cmake_install.cmake")
  include("/home/elmeabde/sawtooth1/projects/moose/framework/contrib/wasp/build/waspsiren/cmake_install.cmake")
  include("/home/elmeabde/sawtooth1/projects/moose/framework/contrib/wasp/build/waspexpr/cmake_install.cmake")
  include("/home/elmeabde/sawtooth1/projects/moose/framework/contrib/wasp/build/waspson/cmake_install.cmake")
  include("/home/elmeabde/sawtooth1/projects/moose/framework/contrib/wasp/build/waspddi/cmake_install.cmake")
  include("/home/elmeabde/sawtooth1/projects/moose/framework/contrib/wasp/build/waspjson/cmake_install.cmake")
  include("/home/elmeabde/sawtooth1/projects/moose/framework/contrib/wasp/build/wasphive/cmake_install.cmake")
  include("/home/elmeabde/sawtooth1/projects/moose/framework/contrib/wasp/build/wasphit/cmake_install.cmake")
  include("/home/elmeabde/sawtooth1/projects/moose/framework/contrib/wasp/build/wasphalite/cmake_install.cmake")
  include("/home/elmeabde/sawtooth1/projects/moose/framework/contrib/wasp/build/wasplsp/cmake_install.cmake")

endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "/home/elmeabde/sawtooth1/projects/moose/framework/contrib/wasp/build/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
