cmake_minimum_required(VERSION 2.6)

# Source files
SET(BEQUNGE-SOURCES
	main.cpp
	mainwindow.cpp
	interpreter.cpp
	fungespace.cpp
	glview.cpp
	stringmodewidget.cpp
	fungeconsole.cpp
	fungecommand.cpp
	stackstack.cpp
	coord.cpp
	extradimensions.cpp
	sound.cpp
	sysinfo.cpp
	console.cpp
	glfont.cpp
)

# Header files that have Q_OBJECT in
SET(BEQUNGE-MOC-HEADERS
	fungeconsole.h
	fungespace.h
	glview.h
	interpreter.h
	mainwindow.h
	sound.h
	stringmodewidget.h
	console.h
)

# UI files
SET(BEQUNGE-UI
	mainwindow.ui
)

# Resource files
SET(BEQUNGE-RESOURCES
	resources.qrc
)

QT4_WRAP_CPP(BEQUNGE-SOURCES-MOC ${BEQUNGE-MOC-HEADERS})
QT4_WRAP_UI(BEQUNGE-SOURCES-UI ${BEQUNGE-UI})
QT4_ADD_RESOURCES(BEQUNGE-SOURCES-RESOURCE ${BEQUNGE-RESOURCES})

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})

INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../Particle")
INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../libcommongl")
INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../ParticleLib")
LINK_DIRECTORIES("${CMAKE_CURRENT_BINARY_DIR}/../ParticleLib")
LINK_DIRECTORIES("${CMAKE_CURRENT_BINARY_DIR}/../libcommongl")

ADD_EXECUTABLE(bequnge ${BEQUNGE-SOURCES} ${BEQUNGE-SOURCES-MOC} ${BEQUNGE-SOURCES-UI} ${BEQUNGE-SOURCES-RESOURCE})
TARGET_LINK_LIBRARIES(bequnge ${QT_LIBRARIES} -lParticle -lcommongl)

ADD_DEPENDENCIES(bequnge Particle commongl)

INSTALL(TARGETS bequnge
	RUNTIME DESTINATION bin)

SET (EXECUTABLE_OUTPUT_PATH ..)
IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	ADD_CUSTOM_TARGET(bundle
		cp -n "${CMAKE_CURRENT_SOURCE_DIR}/../Info.plist" .
		COMMAND cp -f "${CMAKE_CURRENT_SOURCE_DIR}/icons/Bequnge.icns" .
		COMMAND cp -f "${CMAKE_CURRENT_SOURCE_DIR}/../qt.conf" .
		COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/../mkbundle.sh"
		COMMAND "${CMAKE_MAKE_PROGRAM}" -f "${CMAKE_CURRENT_SOURCE_DIR}/../mkdmg.make"
		DEPENDS bequnge
		WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/..)

	SET_TARGET_PROPERTIES(bundle
		PROPERTIES EXCLUDE_FROM_DEFAULT_BUILD 1)

ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
