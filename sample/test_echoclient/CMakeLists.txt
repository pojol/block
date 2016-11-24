PROJECT(test_echoclient)

#引用头文件目录
INCLUDE_DIRECTORIES(
	${GSF_SOURCE_DIR}/3rd/libevent-2.0.22-stable/include
	${GSF_SOURCE_DIR}/core/network
	${GSF_SOURCE_DIR}/utils
)

#引用库目录
LINK_DIRECTORIES(
	${GSF_SOURCE_DIR}/LIB_D
)

FILE(GLOB_RECURSE SRC "*.cpp" "*.h")
ADD_EXECUTABLE(test_echoclient ${SRC})

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

	TARGET_LINK_LIBRARIES(test_echoclient ws2_32.lib libevent.lib libevent_core.lib libevent_extras.lib network.lib utils.lib)
	
endif(MSVC)


