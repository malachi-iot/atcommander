cmake_minimum_required(VERSION 3.2)

project(test_atcommander)

ADD_DEFINITIONS(-std=c++11)

ADD_DEFINITIONS(-D__POSIX__)
ADD_DEFINITIONS(-DDEBUG_SIMULATED)
ADD_DEFINITIONS(-DDEBUG_ATC_STRICT)
ADD_DEFINITIONS(-DFEATURE_IOSTREAM)

INCLUDE_DIRECTORIES(lib/Catch/include)
INCLUDE_DIRECTORIES(lib/atcommander)
INCLUDE_DIRECTORIES(lib/atcommander/target)
INCLUDE_DIRECTORIES(lib/util.embedded/src)

file(GLOB atcommander_source "${PROJECT_SOURCE_DIR}/lib/atcommander/*.cpp"
    "${PROJECT_SOURCE_DIR}/lib/atcommander/target/*.cpp"
    "${PROJECT_SOURCE_DIR}/lib/atcommander/*.h")

#    "${PROJECT_SOURCE_DIR}/lib/atcommander/CMakeLists.txt")

# don't know why, but *.h?? ends up excluding a lot of files
file(GLOB_RECURSE util_embedded_source "${PROJECT_SOURCE_DIR}/lib/util.embedded/src/*.c??"
    "${PROJECT_SOURCE_DIR}/lib/util.embedded/src/*.h"
    "${PROJECT_SOURCE_DIR}/lib/util.embedded/src/*.hpp")

add_library(atcommander ${atcommander_source})
add_library(util_embedded ${util_embedded_source})

#file(GLOB Catch_source "lib/Catch/*.hpp")
#add_library(Catch ${Catch_source})
#set_target_properties(Catch PROPERTIES LINKER_LANGUAGE CPP)

file(GLOB main_source "*.cpp")

add_executable(${PROJECT_NAME} ${main_source})
target_link_libraries(${PROJECT_NAME} atcommander util_embedded)

