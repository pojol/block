PROJECT(network)

#引用头文件目录
if(WIN32)
    INCLUDE_DIRECTORIES(
            ${BLOCK_SOURCE_DIR}/3rd/Libevent/include
			${BLOCK_SOURCE_DIR}/3rd/fmt
			${BLOCK_SOURCE_DIR}/3rd/glog/src
            ${BLOCK_SOURCE_DIR}/
    )
endif()

if(UNIX)
    INCLUDE_DIRECTORIES(
       	${BLOCK_SOURCE_DIR}/lib/libevent/include
	    ${BLOCK_SOURCE_DIR}/lib/fmt/include
	    ${BLOCK_SOURCE_DIR}/lib/glog/include
       	${BLOCK_SOURCE_DIR}/
    )
endif()

if(APPLE)
    INCLUDE_DIRECTORIES(
            /usr/local/Cellar/libevent/2.1.8/include
            ${BLOCK_SOURCE_DIR}/3rd/fmt
            /usr/local/Cellar/gflags/2.2.1/include
            /usr/local/Cellar/glog/0.3.5_3/include
            ${BLOCK_SOURCE_DIR}/
			)
endif()

Add_Definitions("-DGLOG_NO_ABBREVIATED_SEVERITIES")

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

	SET(LIBRARY_OUTPUT_PATH ${BLOCK_SOURCE_DIR}/lib/win)        
        #TARGET_LINK_LIBRARIES(network ws2_32.lib)      项目本身是静态库不需要链接静态库
elseif(UNIX)
        SET(LIBRARY_OUTPUT_PATH ${BLOCK_SOURCE_DIR}/lib/)
elseif(APPLE)
	SET(LIBRARY_OUTPUT_PATH ${BLOCK_SOURCE_DIR}/lib/)
endif()
