cmake_minimum_required(VERSION 3.2)
project(console)

ADD_DEFINITIONS(-std=c++11)

set(sys_lib_path "/usr/local/lib")
set(sys_include_path /usr/local/include)

set(CMAKE_FIND_LIBRARY_PREFIXES lib)
set(CMAKE_FIND_LIBRARY_SUFFIXES .a)

message("Debug: ${sys_include_path}")

INCLUDE_DIRECTORIES(${sys_include_path}/libusb-1.0)
INCLUDE_DIRECTORIES(lib/libusbpp/headers)

FILE(GLOB_RECURSE src_libusbpp "lib/libusbpp/src/*" "lib/libusbpp/headers/*.hpp")

#find_package(PkgConfig REQUIRED)
#pkg_check_modules(PCLIBXML libxml-2.0)
#find_package(LibUSB-1.0 REQUIRED)

add_library(libusbpp ${src_libusbpp})

find_path(libusb_include "libusb-1.0/libusb.h" HINTS ${sys_include_path})
find_library(libusb libusb-1.0.a HINTS ${sys_lib_path})

set(IOKIT_LIB "-framework IOKit")
set(MACCORE_LIB "-framework Cocoa")

add_executable(${PROJECT_NAME} "main.cpp")

target_link_libraries(${PROJECT_NAME} ${libusb} ${IOKIT_LIB} ${MACCORE_LIB} ${libusbpp})
