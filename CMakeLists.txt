INCLUDE(UsePkgConfig)
INCLUDE(CheckIncludeFileCXX)

# Include our custom cmake modules
SET(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMake)

# Discover native tr1 support
CHECK_INCLUDE_FILE_CXX(tr1/array TR1)
IF(NOT TR1)
	MESSAGE("No native TR1 support found - Looking for Boost instead")
	FIND_PACKAGE(Boost REQUIRED)
ENDIF(NOT TR1)

FIND_PACKAGE(Qt4 REQUIRED)
FIND_PACKAGE(OpenGL REQUIRED)

# Fail if Qt doesn't have OpenGL support
IF (NOT QT_QTOPENGL_FOUND)
	MESSAGE(FATAL_ERROR "Qt OpenGL libraries were not found.")
ENDIF (NOT QT_QTOPENGL_FOUND)

# Set up definitions and paths
ADD_DEFINITIONS(${QT_DEFINITIONS})
LINK_DIRECTORIES(${QT_LIBRARY_DIR} ${Boost_LIBRARY_DIRS} ${PHONON_LIBRARY})
INCLUDE_DIRECTORIES(${QT_INCLUDES} ${Boost_INCLUDE_DIRS})

# Subdirectories
ADD_SUBDIRECTORY(ParticleLib)
ADD_SUBDIRECTORY(src)
ADD_SUBDIRECTORY(tests)

# Uninstall support
CONFIGURE_FILE(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
  IMMEDIATE @ONLY)

ADD_CUSTOM_TARGET(uninstall
  "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")
