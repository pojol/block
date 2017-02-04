
PROJECT(test_network)

#引用头文件目录
if(WIN32)
        INCLUDE_DIRECTORIES(
                ${GSF_SOURCE_DIR}/3rd/libevent-2.0.22-stable/include
                ${GSF_SOURCE_DIR}/core/network
				${GSF_SOURCE_DIR}/core
                ${GSF_SOURCE_DIR}/utils
        )
endif()
if(UNIX)
        INCLUDE_DIRECTORIES(
                ${GSF_SOURCE_DIR}/3rd/libevent/include
                ${GSF_SOURCE_DIR}/core/network
				${GSF_SOURCE_DIR}/core
                ${GSF_SOURCE_DIR}/utils
        )
endif()
if(APPLE)
    INCLUDE_DIRECTORIES(
            ${GSF_SOURCE_DIR}/3rd/libevent-2.0.22-stable/include
            ${GSF_SOURCE_DIR}/core/network
            ${GSF_SOURCE_DIR}/core
            ${GSF_SOURCE_DIR}/utils
    )
endif()

#引用库目录
if(WIN32)
        LINK_DIRECTORIES(
                ${GSF_SOURCE_DIR}/lib/windows/Debug
        )
elseif(UNIX)
        LINK_DIRECTORIES(
				${GSF_SOURCE_DIR}/lib/linux
        )
elseif(APPLE)
endif()

FILE(GLOB_RECURSE SRC "*.cpp" "*.h")
ADD_EXECUTABLE(test_network ${SRC})

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

        TARGET_LINK_LIBRARIES(test_network stream.lib libevent.lib libevent_core.lib libevent_extras.lib network.lib utils.lib ws2_32.lib)

elseif(UNIX)
        TARGET_LINK_LIBRARIES(test_network libstream.a libnetwork.a libevent.a libevent_core.a libevent_extra.a libutils.a)
elseif(APPLE)

endif()
