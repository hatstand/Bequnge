INCLUDE_DIRECTORIES(.)

SET(LIBCOMMONGL-SOURCES
	frustum.cpp
	opengl.cpp
	shader.cpp
	smoothvar.cpp
	texture.cpp
	postprocessing.cpp
	commonutils.cpp
	matrix.cpp
	vector.cpp
)

SET(LIBCOMMONGL-RESOURCES
	commonshaders.qrc
)

QT4_ADD_RESOURCES(LIBCOMMONGL-SOURCES-RESOURCE ${LIBCOMMONGL-RESOURCES})

ADD_LIBRARY(commongl STATIC
	${LIBCOMMONGL-SOURCES}
	${LIBCOMMONGL-SOURCES-RESOURCE}
)
