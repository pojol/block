PROJECT(mysql_proxy)

if(WIN32)
    #引用头文件目录
    INCLUDE_DIRECTORIES(
            ${GSF_SOURCE_DIR}/
            ${GSF_SOURCE_DIR}/3rd/glog-0.3.4/src
			${GSF_SOURCE_DIR}/3rd/redis-win-3.0.504/deps/hiredis
			${GSF_SOURCE_DIR}/3rd/mysql_connect/include
    )

    #引用库目录
    LINK_DIRECTORIES(
    )

endif()
if(UNIX)
    #引用头文件目录
    INCLUDE_DIRECTORIES(
            ${GSF_SOURCE_DIR}/
            ${GSF_SOURCE_DIR}/lib/mysql_connector/include
    )

    #引用库目录
    LINK_DIRECTORIES(
            ${GSF_SOURCE_DIR}/lib/
    )
endif()


FILE(GLOB_RECURSE SRC "*.cpp" "*.h" "*.hpp")
ADD_LIBRARY(mysql_proxy STATIC ${SRC})

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
elseif(UNIX)
	SET(LIBRARY_OUTPUT_PATH ${GSF_SOURCE_DIR}/lib/)
elseif(APPLE)
endif()


