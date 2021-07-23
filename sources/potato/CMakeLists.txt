cmake_minimum_required (VERSION 3.20)

file(GLOB_RECURSE POTATO_HPP RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} CONFIGURE_DEPENDS *.mpp)
file(GLOB_RECURSE POTATO_CPP RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} CONFIGURE_DEPENDS *.cpp)

add_library(potato_lib STATIC ${POTATO_CPP} ${POTATO_HPP})

set_source_files_properties(${POTATO_HPP} PROPERTIES
                        LANGUAGE CXX
                        COMPILE_FLAGS "/interface"
)

target_include_directories(potato_lib
                        PRIVATE     "${Vulkan_INCLUDE_DIR}"
                        PUBLIC      "${CMAKE_CURRENT_BINARY_DIR}/version"
)

configure_file("version.hpp.in" "version/version.hpp" @ONLY NEWLINE_STYLE LF)

target_link_libraries(potato_lib PRIVATE modules)
