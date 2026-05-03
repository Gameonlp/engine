cmake_minimum_required(VERSION 3.16)
set(PROJECT_NAME )

if(NOT DEFINED PROJECT_NAME OR PROJECT_NAME STREQUAL "")
    message(FATAL_ERROR "PROJECT_NAME is not set. Please define -DPROJECT_NAME=<name>")
endif()

configure_file(
    cmake/Project.cmake.in
    ${CMAKE_BINARY_DIR}/Generated.cmake
    @ONLY
)

include(${CMAKE_BINARY_DIR}/Generated.cmake)
