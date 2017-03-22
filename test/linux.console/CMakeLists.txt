cmake_minimum_required(VERSION 3.2)

set(sys_lib_path "/usr/local/lib")
set(sys_include_path /usr/local/include)

set(CMAKE_FIND_LIBRARY_PREFIXES lib)
set(CMAKE_FIND_LIBRARY_SUFFIXES .a)

message("Debug: ${sys_include_path}")

INCLUDE_DIRECTORIES(${sys_include_path})
INCLUDE_DIRECTORIES(/usr/local/include/libusb-1.0)

#find_package(PkgConfig REQUIRED)
#pkg_check_modules(PCLIBXML libxml-2.0)
#find_package(libusb-1.0 REQUIRED)
#add_library(libusb "${sys_lib_path}/libusb-1.0.a")

find_path(libusb_include "libusb.h" HINTS ${sys_include_path})
find_library(libusb libusb-1.0.a HINTS ${sys_lib_path})

#find_library(IOKIT_LIB IOKit)
#find_package(IOKit)
set(IOKIT_LIB "-framework IOKit")
set(MACCORE_LIB "-framework Cocoa")

project(console)
add_executable(${PROJECT_NAME} "main.cpp")

target_link_libraries(${PROJECT_NAME} ${libusb} ${IOKIT_LIB} ${MACCORE_LIB})
