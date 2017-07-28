PROJECT(test_echo_client)

#引用头文件目录
if(WIN32)
	INCLUDE_DIRECTORIES(
		${GSF_SOURCE_DIR}/3rd/libevent-2.0.22-stable/include
		${GSF_SOURCE_DIR}/3rd/protobuf-3.2.0/src
		${GSF_SOURCE_DIR}/
		${GSF_SOURCE_DIR}/modules
		${GSF_SOURCE_DIR}/3rd/lua-5.3.4/src
		${GSF_SOURCE_DIR}/3rd/sol2-2.16.0
	)
endif()
if(UNIX)
	INCLUDE_DIRECTORIES(
		${GSF_SOURCE_DIR}/lib/libevent/include
		${GSF_SOURCE_DIR}/
		${GSF_SOURCE_DIR}/modules
		${GSF_SOURCE_DIR}/lib/glog/include
		${GSF_SOURCE_DIR}/lib/lua
		${GSF_SOURCE_DIR}/3rd/sol2
		${GSF_SOURCE_DIR}/lib/protobuf/include
		)
endif()
if(APPLE)
    INCLUDE_DIRECTORIES(
            ${GSF_SOURCE_DIR}/3rd/libevent-2.0.22-stable/include
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
			${GSF_SOURCE_DIR}/lib/libevent/lib
			${GSF_SOURCE_DIR}/lib/glog/lib
			${GSF_SOURCE_DIR}/lib/protobuf/lib
	)
endif()
if(APPLE)
endif()

FILE(GLOB_RECURSE SRC "*.cpp" "*.h" "*.cc" "*.lua")
ADD_EXECUTABLE(test_echo_client ${SRC})

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

        TARGET_LINK_LIBRARIES(test_echo_client libevent.lib libevent_core.lib libevent_extras.lib network.lib gsf_core.lib ws2_32.lib glog.lib log.lib lua53.lib lua_proxy.lib libprotobufd.lib)
endif()
if(UNIX)
	if ("${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}" LESS 3.1)
		find_package(Threads)
		target_link_libraries(test_echo_client ${CMAKE_THREAD_LIBS_INIT})
		set(CMAKE_EXE_LINKER_FLAGS "-ldl")
	else()
		set(THREADS_PREFER_PTHREAD_FLAG ON)
		set(CMAKE_EXE_LINKER_FLAGS "-dl")
		message(STATUS "link -dl")

		find_package(Threads REQUIRED)
		target_link_libraries(test_echo_client Threads::Threads)
	endif()

    TARGET_LINK_LIBRARIES(test_echo_client libgsf_core.a libnetwork.a libevent_core.a libevent_extra.a libtimer.a liblog.a libglog.a liblua_proxy.a liblua.a libprotobuf.a)
endif()
if(APPLE)

endif()
