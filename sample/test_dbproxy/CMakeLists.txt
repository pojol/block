PROJECT(test_dbproxy)

#引用头文件目录
if(WIN32)
	INCLUDE_DIRECTORIES(
			${GSF_SOURCE_DIR}/3rd/protobuf-3.2.0/src
			${GSF_SOURCE_DIR}/3rd/libevent-2.0.22-stable/include
			${GSF_SOURCE_DIR}/core
			${GSF_SOURCE_DIR}/modules
			${GSF_SOURCE_DIR}/common
	)
endif()
if(UNIX)
	INCLUDE_DIRECTORIES(
			${GSF_SOURCE_DIR}/3rd/libevent/include
			${GSF_SOURCE_DIR}/core
			${GSF_SOURCE_DIR}/modules
			${GSF_SOURCE_DIR}/common
		)
endif()
if(APPLE)
    INCLUDE_DIRECTORIES(
            ${GSF_SOURCE_DIR}/3rd/libevent-2.0.22-stable/include
            ${GSF_SOURCE_DIR}/core
			${GSF_SOURCE_DIR}/modules
    		${GSF_SOURCE_DIR}/common
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
			${GSF_SOURCE_DIR}/lib/linux
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

        TARGET_LINK_LIBRARIES(test_dbproxy libevent.lib libevent_core.lib libevent_extras.lib network.lib module.lib event.lib ws2_32.lib libprotocd.lib libprotobufd.lib glog.lib log.lib)
endif()
if(UNIX)
        TARGET_LINK_LIBRARIES(test_dbproxy  libnetwork.a libevent.a libevent_core.a libevent_extra.a libtimer.a libprotobuf.a libmodule.a liblog.a libglog.a)
endif()
if(APPLE)

endif()
