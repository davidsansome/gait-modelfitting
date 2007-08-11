FIND_PACKAGE(Qt4 REQUIRED)
FIND_PACKAGE(OpenGL REQUIRED)
FIND_PACKAGE(Boost REQUIRED)

# Include our custom cmake modules
SET(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMake)

FIND_PACKAGE(CG)

IF (NOT CG_FOUND)
	MESSAGE(FATAL_ERROR "The NVIDIA Cg toolkit was not found.  Download it from http://developer.nvidia.com/object/cg_toolkit.html")
ENDIF (NOT CG_FOUND)

# Set up definitions and paths
ADD_DEFINITIONS(${QT_DEFINITIONS})
LINK_DIRECTORIES(${QT_LIBRARY_DIR})
INCLUDE_DIRECTORIES(${QT_INCLUDES})

# Subdirectories
ADD_SUBDIRECTORY(src)

# Uninstall support
CONFIGURE_FILE(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
  IMMEDIATE @ONLY)

ADD_CUSTOM_TARGET(uninstall
  "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")
