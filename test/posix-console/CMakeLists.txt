cmake_minimum_required(VERSION 3.2)

project(posix-console)

ADD_DEFINITIONS(-std=c++11)

ADD_DEFINITIONS(-DFEATURE_IOSTREAM)
ADD_DEFINITIONS(-D__POSIX__)

#include(lib/atcommander/CMakeLists.txt)
INCLUDE_DIRECTORIES(lib/atcommander)
INCLUDE_DIRECTORIES(lib/util.embedded/src)


file(GLOB_RECURSE util_embedded_source "${PROJECT_SOURCE_DIR}/lib/util.embedded/src/*.c??"
    "${PROJECT_SOURCE_DIR}/lib/util.embedded/src/*.h"
    "${PROJECT_SOURCE_DIR}/lib/util.embedded/src/*.hpp")

file(GLOB src_atcommander "lib/atcommander/*.cpp"
    "lib/atcommander/*.h")

add_library(atcommander ${src_atcommander})
add_library(util_embedded ${util_embedded_source})

add_executable(${PROJECT_NAME} "main.cpp")

target_link_libraries(${PROJECT_NAME} atcommander util_embedded)
