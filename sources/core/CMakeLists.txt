cmake_minimum_required (VERSION 3.20)


file(GLOB_RECURSE CORE_HPP RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} CONFIGURE_DEPENDS *.hpp)
file(GLOB_RECURSE CORE_CPP RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} CONFIGURE_DEPENDS *.cpp)

add_library(core STATIC ${CORE_CPP} ${CORE_HPP})

message("bin ${PROJECT_BINARY_DIR}")

target_include_directories(core
                        PUBLIC      "${PROJECT_BINARY_DIR}/sources/version"
                        PRIVATE     "${CMAKE_CURRENT_SOURCE_DIR}"
                        INTERFACE   "${CMAKE_CURRENT_SOURCE_DIR}/.."
)
