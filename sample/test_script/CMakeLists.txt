PROJECT(test_script)

#引用头文件目录
if(WIN32)
	INCLUDE_DIRECTORIES(
		${GSF_SOURCE_DIR}/3rd/libevent-2.0.22-stable/include
		${GSF_SOURCE_DIR}/3rd/protobuf-3.2.0/src
		${GSF_SOURCE_DIR}/core
		${GSF_SOURCE_DIR}/modules
		${GSF_SOURCE_DIR}/common
		${GSF_SOURCE_DIR}/3rd/lua-5.3.4/src
		${GSF_SOURCE_DIR}/3rd/sol2-2.16.0
	)
endif()
if(UNIX)
	INCLUDE_DIRECTORIES(
		${GSF_SOURCE_DIR}/core
		${GSF_SOURCE_DIR}/common
		${GSF_SOURCE_DIR}/modules
		${GSF_SOURCE_DIR}/3rd/lua-5.3.4/src
		${GSF_SOURCE_DIR}/3rd/sol2-2.16.0
	)
endif()
if(APPLE)
	INCLUDE_DIRECTORIES(
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
	set(CMAKE_EXE_LINKER_FLAGS "-llua -ldl")

	LINK_DIRECTORIES(
	${GSF_SOURCE_DIR}/lib/linux
	)
endif()
if(APPLE)
endif()

FILE(GLOB_RECURSE SRC "*.cpp" "*.h")
ADD_EXECUTABLE(test_script ${SRC})

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

        TARGET_LINK_LIBRARIES(test_script ws2_32.lib libevent.lib libevent_core.lib libevent_extras.lib module.lib event.lib lua_proxy.lib lua53.lib glog.lib log.lib timer.lib libprotocd.lib libprotobufd.lib network.lib)
elseif(UNIX)
	TARGET_LINK_LIBRARIES(test_script liblog.a libglog.a libmodule.a libevent.a libtimer.a liblua_proxy.a liblua.a)
elseif(APPLE)
endif()
