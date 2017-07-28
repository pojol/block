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
		${GSF_SOURCE_DIR}/lib/glog/include
		${GSF_SOURCE_DIR}/lib/lua
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
		${GSF_SOURCE_DIR}/lib
		${GSF_SOURCE_DIR}/lib/lua
		${GSF_SOURCE_DIR}/lib/glog/lib
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
	if ("${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}" LESS 3.1)
		target_link_libraries(test_timer ${CMAKE_THREAD_LIBS_INIT})
	else()
		target_link_libraries(test_timer Threads::Threads)
	endif()

    TARGET_LINK_LIBRARIES(test_timer liblog.a libglog.a libgsf_core.a liblua_proxy.a liblua.a libtimer.a -ldl)
elseif(APPLE)
endif()
