PROJECT(GSF)

if(WIN32)
	
elseif(UNIX)
    set(CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")
    message(STATUS "optional:-std=c++11")
elseif(APPLE)
    set(CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")
    message(STATUS "optional:-std=c++11")
endif()

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

ADD_SUBDIRECTORY(core)
ADD_SUBDIRECTORY(sample)
ADD_SUBDIRECTORY(utils)
