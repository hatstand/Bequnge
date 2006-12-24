INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../Particle)

ADD_LIBRARY(Particle STATIC
	ActionsAPI.cpp
	Actions.cpp
	OtherAPI.cpp
	PInternalState.cpp
)
