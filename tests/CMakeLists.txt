INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})
INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../src")
INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../libcommongl")
LINK_DIRECTORIES("${CMAKE_CURRENT_BINARY_DIR}/../libcommongl")

SET(FUNGESPACE-SOURCES
	testfungespace.cpp
	../src/fungespace.cpp
	../src/coord.cpp
)
SET(FUNGESPACE-MOC-HEADERS
	testfungespace.h
	../src/fungespace.h
)
SET (QT_USE_QTTEST 1)
SET (QT_USE_QTOPENGL 0)
INCLUDE(${QT_USE_FILE})

QT4_WRAP_CPP(FUNGESPACE-SOURCES-MOC ${FUNGESPACE-MOC-HEADERS})
ADD_EXECUTABLE(testfungespace ${FUNGESPACE-SOURCES} ${FUNGESPACE-SOURCES-MOC})
TARGET_LINK_LIBRARIES(testfungespace ${QT_LIBRARIES})

SET(COORD-SOURCES
	testcoord.cpp
	../src/coord.cpp
)
SET(COORD-MOC-HEADERS
	testcoord.h
)

QT4_WRAP_CPP(COORD-SOURCES-MOC ${COORD-MOC-HEADERS})
ADD_EXECUTABLE(testcoord ${COORD-SOURCES} ${COORD-SOURCES-MOC})
TARGET_LINK_LIBRARIES(testcoord ${QT_LIBRARIES})


SET(VECTOR-SOURCES
	testvector.cpp
)

SET(VECTOR-MOC-HEADERS
	testvector.h
)

QT4_WRAP_CPP(VECTOR-SOURCES-MOC ${VECTOR-MOC-HEADERS})
ADD_EXECUTABLE(testvector ${VECTOR-SOURCES} ${VECTOR-SOURCES-MOC})
TARGET_LINK_LIBRARIES(testvector ${QT_LIBRARIES} -lcommongl)

SET(MATRIX-SOURCES
	testmatrix.cpp
)

SET(MATRIX-MOC-HEADERS
	testmatrix.h
)

QT4_WRAP_CPP(MATRIX-SOURCES-MOC ${MATRIX-MOC-HEADERS})
ADD_EXECUTABLE(testmatrix ${MATRIX-SOURCES} ${MATRIX-SOURCES-MOC})
TARGET_LINK_LIBRARIES(testmatrix ${QT_LIBRARIES} ${OPENGL_gl_LIBRARY} -lcommongl)

SET(STACK-SOURCES
	../src/stackstack.cpp
	../src/interpreter.cpp
	../src/sysinfo.cpp
	../src/coord.cpp
	../src/fungespace.cpp
	teststackstack.cpp
)

SET(STACK-MOC-HEADERS
	../src/interpreter.h
	../src/fungespace.h
	teststackstack.h
)

QT4_WRAP_CPP(STACK-SOURCES-MOC ${STACK-MOC-HEADERS})
ADD_EXECUTABLE(teststackstack ${STACK-SOURCES} ${STACK-SOURCES-MOC})
TARGET_LINK_LIBRARIES(teststackstack ${QT_LIBRARIES})

SET(INTERPRETER-SOURCES
	../src/fungespace.cpp
	../src/interpreter.cpp
	../src/stackstack.cpp
	../src/sysinfo.cpp
	../src/coord.cpp
	testinterpreter.cpp
)

SET(INTERPRETER-MOC-HEADERS
	../src/fungespace.h
	../src/interpreter.h
	testinterpreter.h
)

QT4_WRAP_CPP(INTERPRETER-SOURCES-MOC ${INTERPRETER-MOC-HEADERS})
ADD_EXECUTABLE(testinterpreter ${INTERPRETER-SOURCES} ${INTERPRETER-SOURCES-MOC})
TARGET_LINK_LIBRARIES(testinterpreter ${QT_LIBRARIES})

ADD_CUSTOM_TARGET(test
	./testcoord
	COMMAND ./testfungespace
	COMMAND ./testvector
	COMMAND ./testmatrix
	COMMAND ./teststackstack
	COMMAND ./testinterpreter
	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/..
	DEPENDS testcoord testfungespace testvector testmatrix teststackstack testinterpreter)

SET (EXECUTABLE_OUTPUT_PATH ..)
