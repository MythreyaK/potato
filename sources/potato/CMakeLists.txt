cmake_minimum_required (VERSION 3.20)

file(GLOB_RECURSE POTATO_HPP RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} CONFIGURE_DEPENDS *.hpp)
file(GLOB_RECURSE POTATO_CPP RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} CONFIGURE_DEPENDS *.cpp)

add_library(potato_lib STATIC ${POTATO_CPP} ${POTATO_HPP})

target_include_directories(potato_lib
                        PUBLIC      "${CMAKE_CURRENT_BINARY_DIR}/version"
                        PUBLIC      "${Vulkan_INCLUDE_DIR}"
                        PRIVATE     "${CMAKE_CURRENT_SOURCE_DIR}/graphics"
                        PRIVATE     "${CMAKE_CURRENT_SOURCE_DIR}/core"
                        PRIVATE     "${CMAKE_CURRENT_SOURCE_DIR}"
                        INTERFACE   "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_link_libraries(potato_lib
                    PUBLIC Vulkan::Vulkan
)

configure_file("version.hpp.in" "version/version.hpp" @ONLY NEWLINE_STYLE LF)

target_precompile_headers(potato_lib PUBLIC "graphics/vkinclude/vulkan.hpp")
