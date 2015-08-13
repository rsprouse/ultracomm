CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(ultracomm)

INCLUDE(${CMAKE_CURRENT_SOURCE_DIR}/common.cmake)

#### Variables that might need to be changed. ####

## Ultrasonix SDK ##
# Location of Ultrasonix SDK on your system.
SET(ULTRASONIX_SDK_PATH "C:/src/sdk611")

# The Ultrasonix SDK version number
SET(Ulterius_sdk_VERSION_MAJOR "6")
SET(Ulterius_sdk_VERSION_MINOR "1")
SET(Ulterius_sdk_VERSION_PATCH "1")

# Name of ulterius library. This might be ulterius.lib or ulterius_old.lib,
# depending on your version of the Ultrasonix SDK.
# ulterius.lib (5.7.4)
# ulterius_old.lib (6.x)
SET(ULTERIUS_LIB_NAME "ulterius_old.lib")
## end Ultrasonix SDK ##

## Boost configuration ## 
# Location of boost on your system.
SET(Boost_INCLUDE_DIR "C:/local/boost_1_55_0")

# Add the location of the boost libaries.
# Visual Studio 2010
#SET(Boost_LIBRARYDIR "C:/local/boost_1_55_0/lib32-msvc-10.0")
# Visual Studio 12 2013
SET(Boost_LIBRARYDIR "C:/local/boost_1_55_0/lib32-msvc-12.0")
# end Boost configuration ##

# ultracomm version number (only ultracomm developers should change this).
SET(Ultracomm_VERSION_MAJOR "0")
SET(Ultracomm_VERSION_MINOR "2")
SET(Ultracomm_VERSION_PATCH "1")

#### End of variables to be changed. ####


# Concatenate ultracomm version string
SET(Ultracomm_VERSION "${Ultracomm_VERSION_MAJOR}.${Ultracomm_VERSION_MINOR}.${Ultracomm_VERSION_PATCH}")

# Concatenate Ulterius SDK version string
SET(Ulterius_sdk_VERSION "${Ulterius_sdk_VERSION_MAJOR}.${Ulterius_sdk_VERSION_MINOR}.${Ulterius_sdk_VERSION_PATCH}")

# Path to ulterius header files and boost.
INCLUDE_DIRECTORIES("${ULTRASONIX_SDK_PATH}/ulterius/inc" ${Boost_INCLUDE_DIR})
LINK_DIRECTORIES(${LINK_DIRECTORIES} ${Boost_LIBRARYDIR})

#### Begin version_config ####

# See http://www.cmake.org/cmake/help/cmake_tutorial.html for information on
# making version numbers available in the source code.

# configure a header file to pass some of the CMake settings to the source code.
configure_file (
  "${PROJECT_SOURCE_DIR}/version_config.h.in"
  "${PROJECT_BINARY_DIR}/version_config.h"
)

# add the binary tree to the search path for include files so that
# we find version_config.h
include_directories("${PROJECT_BINARY_DIR}")

#### End version_config ####

SUBDIRS(
    src
)
