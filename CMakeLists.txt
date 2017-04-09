PROJECT(GSF)

if(WIN32)

endif()
if(UNIX)
    set(CMAKE_CXX_FLAGS "-std=c++14 ${CMAKE_CXX_FLAGS}")
    message(STATUS "optional:-std=c++1z")
    set(CMAKE_EXE_LINKER_FLAGS "-lrt")
endif()
if(APPLE)
    set(CMAKE_CXX_FLAGS "-std=c++14 ${CMAKE_CXX_FLAGS}")
    message(STATUS "optional:-std=c++11")
endif()


SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)



ADD_SUBDIRECTORY(core)
ADD_SUBDIRECTORY(modules)
ADD_SUBDIRECTORY(sample)
