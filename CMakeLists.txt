cmake_minimum_required(VERSION 2.6)

INCLUDE(UsePkgConfig)

# Include our custom cmake modules
SET(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMake)

# Boost now required for multi_index
FIND_PACKAGE(Boost REQUIRED)

IF (NOT Boost_FOUND)
	MESSAGE(FATAL_ERROR "Boost not found")
ENDIF (NOT Boost_FOUND)

FIND_PACKAGE(Qt4 REQUIRED)
SET(QT_USE_QTOPENGL 1)
SET(QT_USE_PHONON 1)

FIND_PACKAGE(OpenGL REQUIRED)

# Fail if Qt doesn't have OpenGL support
IF (NOT QT_QTOPENGL_FOUND)
	MESSAGE(FATAL_ERROR "Qt OpenGL libraries were not found.")
ENDIF (NOT QT_QTOPENGL_FOUND)

IF (NOT QT_PHONON_FOUND)
	ADD_DEFINITIONS(-DNO_SOUND)
	MESSAGE("Phonon not found. Sound disabled.")
ENDIF (NOT QT_PHONON_FOUND)

# Set up definitions and paths
ADD_DEFINITIONS(${QT_DEFINITIONS})
LINK_DIRECTORIES(${Boost_LIBRARY_DIRS})
INCLUDE(${QT_USE_FILE})
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})

OPTION(TESTS_ENABLED "Build the unit tests." OFF)

# Subdirectories
ADD_SUBDIRECTORY(ParticleLib)
ADD_SUBDIRECTORY(libcommongl)
ADD_SUBDIRECTORY(src)
IF (TESTS_ENABLED)
	ADD_SUBDIRECTORY(tests)
ENDIF (TESTS_ENABLED)

# Uninstall support
CONFIGURE_FILE(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
  IMMEDIATE @ONLY)

ADD_CUSTOM_TARGET(uninstall
  "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")
