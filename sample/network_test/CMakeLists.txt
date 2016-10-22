PROJECT(network_test)

#引用头文件目录
INCLUDE_DIRECTORIES(
	${ES_SOURCE_DIR}/depend/libevent-2.0.22-stable/include
)

#引用库目录
LINK_DIRECTORIES(
	${ES_SOURCE_DIR}/depend/lib
)

FILE(GLOB_RECURSE SRC "*.cpp" ".h")
ADD_EXECUTABLE(network_test ${SRC})

if(MSVC)     
    # Use the static C library for all build types
    foreach(var 
        CMAKE_C_FLAGS CMAKE_C_FLAGS_DEBUG CMAKE_C_FLAGS_RELEASE
        CMAKE_C_FLAGS_MINSIZEREL CMAKE_C_FLAGS_RELWITHDEBINFO
        CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE
        CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO
      )
      if(${var} MATCHES "/MD")
        string(REGEX REPLACE "/MD" "/MT" ${var} "${${var}}")
      endif()
    endforeach()    

	TARGET_LINK_LIBRARIES(network_test ws2_32.lib libevent.lib libevent_core.lib libevent_extras.lib)
	
endif(MSVC)


