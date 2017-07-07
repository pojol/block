PROJECT(test_timer)

#引用头文件目录
if(WIN32)
	INCLUDE_DIRECTORIES(
		${GSF_SOURCE_DIR}/
		${GSF_SOURCE_DIR}/modules
		${GSF_SOURCE_DIR}/3rd/lua-5.3.4/src
		${GSF_SOURCE_DIR}/3rd/sol2-2.16.0
	)
endif()
if(UNIX)
	INCLUDE_DIRECTORIES(
		${GSF_SOURCE_DIR}/
		${GSF_SOURCE_DIR}/modules
		${GSF_SOURCE_DIR}/3rd/lua53/lua-5.3.4/src
		${GSF_SOURCE_DIR}/3rd/sol2
	)
endif()
if(APPLE)
	INCLUDE_DIRECTORIES(
		${GSF_SOURCE_DIR}/
		${GSF_SOURCE_DIR}/modules
    )
endif()


#引用库目录
if(WIN32)
	LINK_DIRECTORIES(
	${GSF_SOURCE_DIR}/lib/win/Debug
	)
endif()
if(UNIX)
	find_package(Threads)
	LINK_DIRECTORIES(
		${GSF_SOURCE_DIR}/lib/linux
		${GSF_SOURCE_DIR}/lib/linux/glog/lib
	)
endif()
if(APPLE)
endif()

FILE(GLOB_RECURSE SRC "*.cpp" "*.h")
ADD_EXECUTABLE(test_timer ${SRC})

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

        TARGET_LINK_LIBRARIES(test_timer ws2_32.lib libevent.lib libevent_core.lib libevent_extras.lib gsf_core.lib timer.lib glog.lib log.lib lua53.lib lua_proxy.lib)
elseif(UNIX)
        TARGET_LINK_LIBRARIES(test_timer ${CMAKE_THREAD_LIBS_INIT} liblog.a libglog.a libgsf_core.a libtimer.a liblua_proxy.a)
elseif(APPLE)
endif()
