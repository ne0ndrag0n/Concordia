cmake_minimum_required(VERSION 3.10)
project(
    Concordia
    VERSION 0.0.4
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_BUILD_TYPE Debug)

add_definitions(-DSOL_SAFE_FUNCTION=1)

file(GLOB_RECURSE CONCORDIA_SRC ${CMAKE_SOURCE_DIR}/src/*.cpp)
add_executable(cncrda64 ${CONCORDIA_SRC})

target_include_directories(cncrda64 PUBLIC ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/lib ${CMAKE_SOURCE_DIR}/lib/nanovg)

find_library(LIB_PTHREAD pthread)
find_library(LIB_TBB tbb)
find_library(LIB_ASSIMP assimp)
find_library(LIB_GLEW GLEW)
find_library(LIB_GL GL)
find_library(LIB_SFML_GRAPHICS sfml-graphics)
find_library(LIB_SFML_WINDOW sfml-window)
find_library(LIB_SFML_SYSTEM sfml-system)
find_library(LIB_TINYXML2 tinyxml2)
find_library(LIB_JSONCPP jsoncpp)
find_library(LIB_LUA lua)
find_library(LIB_DL dl)
find_library(LIB_NANOVG nanovg PATHS ${CMAKE_SOURCE_DIR}/lib/nanovg)

target_link_libraries(
    cncrda64
    ${LIB_PTHREAD}
    ${LIB_TBB}
    ${LIB_ASSIMP}
    ${LIB_GLEW}
    ${LIB_GL}
    ${LIB_SFML_GRAPHICS}
    ${LIB_SFML_WINDOW}
    ${LIB_SFML_SYSTEM}
    ${LIB_TINYXML2}
    ${LIB_JSONCPP}
    ${LIB_LUA}
    ${LIB_DL}
    ${LIB_NANOVG}
)