
PROJECT(module)

#引用头文件目录
if(WIN32)
        INCLUDE_DIRECTORIES(
        )
endif()
if(UNIX)
        INCLUDE_DIRECTORIES(
        )
endif()
if(APPLE)
endif()


#引用库目录
if(WIN32)
        LINK_DIRECTORIES(
        )
elseif(UNIX)
elseif(APPLE)
endif()

FILE(GLOB_RECURSE SRC "*.cpp" "*.h")
ADD_LIBRARY(module STATIC ${SRC})

SET_TARGET_PROPERTIES(module PROPERTIES LINKER_LANGUAGE CXX)

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
