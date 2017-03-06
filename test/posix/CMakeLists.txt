cmake_minimum_required(VERSION 3.6)

project(test_atcommander)

INCLUDE_DIRECTORIES(lib/Catch/include)

#add_library(Catch lib/Catch/include)

#set_target_properties(Catch PROPERTIES LINKER_LANGUAGE CPP)

add_executable(${PROJECT_NAME} "main.cpp")
