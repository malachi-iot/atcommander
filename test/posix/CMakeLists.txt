cmake_minimum_required(VERSION 3.6)

project(test_atcommander)

ADD_DEFINITIONS(-std=c++11)
INCLUDE_DIRECTORIES(lib/Catch/include)
INCLUDE_DIRECTORIES(lib/atcommander)

file(GLOB atcommander_source "${PROJECT_SOURCE_DIR}/lib/atcommander/*.cpp"
    "${PROJECT_SOURCE_DIR}/lib/atcommander/*.h")

add_library(atcommander ${atcommander_source})

#file(GLOB Catch_source "lib/Catch/*.hpp")
#add_library(Catch ${Catch_source})
#set_target_properties(Catch PROPERTIES LINKER_LANGUAGE CPP)

file(GLOB main_source "*.cpp")

add_executable(${PROJECT_NAME} ${main_source})
