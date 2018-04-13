PROJECT(http)

#引用头文件目录
if(WIN32)
    INCLUDE_DIRECTORIES(
            ${BLOCK_SOURCE_DIR}/3rd/Libevent/include
			${BLOCK_SOURCE_DIR}/3rd/fmt
			${BLOCK_SOURCE_DIR}/3rd/cinatra-0.02
			${BLOCK_SOURCE_DIR}/3rd/asio-1.10.6/include
            ${BLOCK_SOURCE_DIR}/
    )
	
	Add_Definitions("-D_WIN32_WINDOWS")
	Add_Definitions("-DASIO_STANDALONE")
endif()
if(UNIX)
    INCLUDE_DIRECTORIES(
            ${BLOCK_SOURCE_DIR}/lib/libevent/include
			${BLOCK_SOURCE_DIR}/lib/fmt/include
            ${BLOCK_SOURCE_DIR}/
			)
endif()
if(APPLE)
    INCLUDE_DIRECTORIES(
            ${BLOCK_SOURCE_DIR}/lib/libevent/include
            ${BLOCK_SOURCE_DIR}/
			)
endif()


#引用库目录
if(WIN32)
    LINK_DIRECTORIES(
            ${BLOCK_SOURCE_DIR}/lib
    )
elseif(UNIX)
    LINK_DIRECTORIES(
            ${BLOCK_SOURCE_DIR}/lib/libevent/lib
            ${BLOCK_SOURCE_DIR}/lib
    )
elseif(APPLE)
	LINK_DIRECTORIES(
		${BLOCK_SOURCE_DIR}/lib/apple/lib
	)
endif()


FILE(GLOB_RECURSE SRC "*.cpp" "*.h" "*.hpp")
ADD_LIBRARY(http STATIC ${SRC})

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

	SET(LIBRARY_OUTPUT_PATH ${BLOCK_SOURCE_DIR}/lib/win)        
elseif(UNIX)
        SET(LIBRARY_OUTPUT_PATH ${BLOCK_SOURCE_DIR}/lib/)
elseif(APPLE)
	SET(LIBRARY_OUTPUT_PATH ${BLOCK_SOURCE_DIR}/lib/)
endif()
