cmake_minimum_required (VERSION 3.20)

add_library(utils STATIC "utils.cpp" "utils.hpp")

target_include_directories(utils
                    PRIVATE     "${CMAKE_CURRENT_SOURCE_DIR}"
                    INTERFACE   "${CMAKE_CURRENT_SOURCE_DIR}/.."
)
