PROJECT(test_dbproxy)

#引用头文件目录
if(WIN32)
	INCLUDE_DIRECTORIES(
			${GSF_SOURCE_DIR}/3rd/protobuf-3.2.0/src
			${GSF_SOURCE_DIR}/3rd/libevent-2.0.22-stable/include
			${GSF_SOURCE_DIR}/
			${GSF_SOURCE_DIR}/modules
	)
endif()
if(UNIX)
	INCLUDE_DIRECTORIES(
			${GSF_SOURCE_DIR}/lib/libevent/include
			${GSF_SOURCE_DIR}/
			${GSF_SOURCE_DIR}/modules
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
			${GSF_SOURCE_DIR}/lib/libevent/lib
			${GSF_SOURCE_DIR}/lib/glog/lib
			${GSF_SOURCE_DIR}/lib/protobuf/lib
	)
endif()
if(APPLE)
endif()

FILE(GLOB_RECURSE SRC "*.cpp" "*.h" "*.cc")
ADD_EXECUTABLE(test_dbproxy ${SRC})

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

        TARGET_LINK_LIBRARIES(test_dbproxy libevent.lib libevent_core.lib libevent_extras.lib network.lib gsf_core.lib ws2_32.lib libprotocd.lib libprotobufd.lib glog.lib log.lib)
endif()
if(UNIX)

	if(THREADS_HAVE_PTHREAD_ARG)
		target_compile_options(test_dbproxy PUBLIC "-pthread")
	endif()
	if(CMAKE_THREAD_LIBS_INIT)
		target_link_libraries(test_dbproxy ${CMAKE_THREAD_LIBS_INIT} libgsf_core.a libnetwork.a libevent.a libevent_core.a libevent_extra.a libtimer.a libprotobuf.a liblog.a libglog.a)
	endif()

endif()
if(APPLE)

endif()
