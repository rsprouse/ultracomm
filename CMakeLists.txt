CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

# Add the location of the boost libaries.
# TODO: this isn't working yet
SET(LINK_DIRECTORIES ${LINK_DIRECTORIES} "C:/local/boost_1_55_0/lib32-msvc-10.0")

PROJECT(ultracomm)

INCLUDE(${CMAKE_CURRENT_SOURCE_DIR}/common.cmake)


# Edit ULTRASONIX_SDK_PATH to match location of Ultrasonix SDK on your system.
SET(ULTRASONIX_SDK_PATH "C:/src/sdk574")

# This might be ulterius.lib or ulterius_old.lib, depending on your version of
# the Ultrasonix SDK.
# ulterius.lib (5.7.4)
# ulterius_old.lib (6.1.0)
SET(ULTERIUS_LIB_NAME "ulterius.lib")
 
# Path to ulterius header files and boost.
INCLUDE_DIRECTORIES("${ULTRASONIX_SDK_PATH}/ulterius/inc" "C:/local/boost_1_55_0")

#### Begin version_config ####

# See http://www.cmake.org/cmake/help/cmake_tutorial.html for information on
# making version numbers available in the source code.

# Edit the ultracomm version number.
SET(Ultracomm_VERSION_MAJOR "0")
SET(Ultracomm_VERSION_MINOR "1")
SET(Ultracomm_VERSION_PATCH "0")
SET(Ultracomm_VERSION "${Ultracomm_VERSION_MAJOR}.${Ultracomm_VERSION_MINOR}.${Ultracomm_VERSION_PATCH}")

# Edit the SDK version number.
SET(Ulterius_sdk_VERSION_MAJOR "5")
SET(Ulterius_sdk_VERSION_MINOR "7")
SET(Ulterius_sdk_VERSION_PATCH "4")
SET(Ulterius_sdk_VERSION "${Ulterius_sdk_VERSION_MAJOR}.${Ulterius_sdk_VERSION_MINOR}.${Ulterius_sdk_VERSION_PATCH}")

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
