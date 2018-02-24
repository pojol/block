PROJECT(network)

#引用头文件目录
if(WIN32)
    INCLUDE_DIRECTORIES(
            ${GSF_SOURCE_DIR}/3rd/Libevent/include
			${GSF_SOURCE_DIR}/3rd/fmt
            ${GSF_SOURCE_DIR}/
    )
endif()
if(UNIX)
    INCLUDE_DIRECTORIES(
            ${GSF_SOURCE_DIR}/lib/libevent/include
			${GSF_SOURCE_DIR}/lib/fmt/include
            ${GSF_SOURCE_DIR}/
			)
endif()
if(APPLE)
    INCLUDE_DIRECTORIES(
            ${GSF_SOURCE_DIR}/lib/libevent/include
            ${GSF_SOURCE_DIR}/
			)
endif()


#引用库目录
if(WIN32)
    LINK_DIRECTORIES(
            ${GSF_SOURCE_DIR}/lib
    )
elseif(UNIX)
    LINK_DIRECTORIES(
            ${GSF_SOURCE_DIR}/lib/libevent/lib
            ${GSF_SOURCE_DIR}/lib
    )
elseif(APPLE)
	LINK_DIRECTORIES(
		${GSF_SOURCE_DIR}/lib/apple/lib
	)
endif()


FILE(GLOB_RECURSE SRC "*.cpp" "*.h")
ADD_LIBRARY(network STATIC ${SRC})

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

	SET(LIBRARY_OUTPUT_PATH ${GSF_SOURCE_DIR}/lib/win)        
        #TARGET_LINK_LIBRARIES(network ws2_32.lib)      项目本身是静态库不需要链接静态库
elseif(UNIX)
        SET(LIBRARY_OUTPUT_PATH ${GSF_SOURCE_DIR}/lib/)
elseif(APPLE)
	SET(LIBRARY_OUTPUT_PATH ${GSF_SOURCE_DIR}/lib/)
endif()
