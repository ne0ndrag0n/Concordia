cmake_minimum_required(VERSION 3.10)
project(
    Concordia
    VERSION 0.0.4
)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_BUILD_TYPE Debug)

option(GCC7_BUILD "Link filesystem with GCC7" ON)
set(JSONCPP_INCLUDE_PATH "" CACHE PATH "")

find_package(Threads REQUIRED)
find_package(Lua 5.3 EXACT REQUIRED)
find_package(GLEW REQUIRED)
set(OpenGL_GL_PREFERENCE LEGACY)
find_package(OpenGL REQUIRED)
find_package(assimp 4.1 REQUIRED)
find_package(TBB 2017.0 REQUIRED)
find_package(SFML 2.4 REQUIRED COMPONENTS graphics window system)

add_definitions(-DSOL_SAFE_FUNCTION=1 -DGLM_FORCE_COMPILER_UNKNOWN)

file(GLOB_RECURSE CONCORDIA_SRC ${CMAKE_SOURCE_DIR}/src/*.cpp)
add_executable(cncrda64 ${CONCORDIA_SRC})

find_path(TINYXML2_INCLUDE_DIR "tinyxml2.h")

find_library(LIB_ASSIMP ${ASSIMP_LIBRARIES})
find_library(LIB_TINYXML2 tinyxml2)
find_library(LIB_NANOVG nanovg PATHS ${CMAKE_SOURCE_DIR}/lib/nanovg)
find_library(LIB_JSONCPP jsoncpp)

target_include_directories(cncrda64 PUBLIC
    ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/lib ${CMAKE_SOURCE_DIR}/lib/nanovg
    ${LUA_INCLUDE_DIR}
    ${GLEW_INCLUDE_DIRS}
    ${OPENGL_INCLUDE_DIR}
    ${ASSIMP_INCLUDE_DIRS}
    ${TBB_INCLUDE_DIRS}
    ${TINYXML2_INCLUDE_DIR}
    ${SFML_INCLUDE_DIR}
    ${JSONCPP_INCLUDE_PATH}
)

target_link_libraries(
    cncrda64
    ${CMAKE_THREAD_LIBS_INIT}
    ${TBB_LIBRARIES}
    ${LIB_ASSIMP}
    ${GLEW_LIBRARIES}
    ${OPENGL_LIBRARIES}
    ${SFML_LIBRARIES}
    ${LIB_TINYXML2}
    ${LIB_JSONCPP}
    ${LUA_LIBRARIES}
    ${LIB_NANOVG}
)

if(GCC7_BUILD)
    target_link_libraries(cncrda64 stdc++fs)
    add_definitions(-DFS_EXPERIMENTAL)
endif()

if ( CMAKE_COMPILER_IS_GNUCC )
    #target_compile_options(cncrda64 PRIVATE "-Wall")
    #target_compile_options(cncrda64 PRIVATE "-Wextra")
    target_compile_options(cncrda64 PRIVATE "-Wreturn-type")
endif()
if ( MSVC )
    target_compile_options(cncrda64 PRIVATE "/W0")
endif()