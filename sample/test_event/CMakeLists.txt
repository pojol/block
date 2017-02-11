PROJECT(test_event)

#引用头文件目录
if(WIN32)
	INCLUDE_DIRECTORIES(
		${GSF_SOURCE_DIR}/plugin
		${GSF_SOURCE_DIR}/common
	)
endif()
if(UNIX)
	INCLUDE_DIRECTORIES(
		)
endif()
if(APPLE)
    INCLUDE_DIRECTORIES(
	)
endif()

#引用库目录
if(WIN32)
	LINK_DIRECTORIES(
	    ${GSF_SOURCE_DIR}/lib/win/Debug
	)
endif()
if(UNIX)
	LINK_DIRECTORIES(
	)
endif()
if(APPLE)
endif()

FILE(GLOB_RECURSE SRC "*.cpp" "*.h")
ADD_EXECUTABLE(test_event ${SRC})

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
	TARGET_LINK_LIBRARIES(test_event stream.lib libevent.lib libevent_core.lib libevent_extras.lib network.lib utils.lib timer.lib ws2_32.lib)
endif()
if(UNIX)
endif()
if(APPLE)

endif()
