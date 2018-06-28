PROJECT(luaAdapter)

if(WIN32)
    #引用头文件目录
    INCLUDE_DIRECTORIES(
            ${BLOCK_SOURCE_DIR}/
            ${BLOCK_SOURCE_DIR}/3rd/lua-5.3.4/src
            ${BLOCK_SOURCE_DIR}/3rd/sol2
			${BLOCK_SOURCE_DIR}/3rd/fmt
			${BLOCK_SOURCE_DIR}/3rd/glog/src
    )
	Add_Definitions("-DGLOG_NO_ABBREVIATED_SEVERITIES")
    #引用库目录
    LINK_DIRECTORIES(
    )

endif()
if(UNIX)
    #引用头文件目录
    INCLUDE_DIRECTORIES(
            ${BLOCK_SOURCE_DIR}/
            ${BLOCK_SOURCE_DIR}/lib/glog/include
            ${BLOCK_SOURCE_DIR}/lib/lua
            ${BLOCK_SOURCE_DIR}/3rd/sol2
            ${BLOCK_SOURCE_DIR}/lib/fmt/include
    )

    #引用库目录
    LINK_DIRECTORIES(
            ${BLOCK_SOURCE_DIR}/lib/
    )
endif()

if(APPLE)
    INCLUDE_DIRECTORIES(
            ${BLOCK_SOURCE_DIR}/3rd/fmt
            /usr/local/Cellar/lua/5.3.4_2/include
            ${BLOCK_SOURCE_DIR}/3rd/sol2
            /usr/local/Cellar/gflags/2.2.1/include
            /usr/local/Cellar/glog/0.3.5_3/include
            ${BLOCK_SOURCE_DIR}/
    )
endif()

FILE(GLOB_RECURSE SRC "*.cpp" "*.h" ".hpp")
ADD_LIBRARY(luaAdapter STATIC ${SRC})

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
	SET(LIBRARY_OUTPUT_PATH ${BLOCK_SOURCE_DIR}/lib)
elseif(APPLE)
endif()


