PROJECT(lua_proxy)

#引用头文件目录
INCLUDE_DIRECTORIES(
	${GSF_SOURCE_DIR}/
	${GSF_SOURCE_DIR}/3rd/glog-0.3.4/src
	${GSF_SOURCE_DIR}/3rd/lua-5.3.4/src
	${GSF_SOURCE_DIR}/3rd/sol2-2.16.0
)

#引用库目录
LINK_DIRECTORIES(
)

FILE(GLOB_RECURSE SRC "*.cpp" "*.h")
ADD_LIBRARY(lua_proxy STATIC ${SRC})

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
	SET(LIBRARY_OUTPUT_PATH ${GSF_SOURCE_DIR}/lib/linux)
elseif(APPLE)
endif()


