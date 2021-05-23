cmake_minimum_required (VERSION 3.16)


file(GLOB_RECURSE RENDER_HPP RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} CONFIGURE_DEPENDS *.hpp)
file(GLOB_RECURSE RENDER_CPP RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} CONFIGURE_DEPENDS *.cpp)

add_library(render STATIC ${RENDER_CPP} ${RENDER_HPP})

target_include_directories(render
                        PRIVATE     "${PROJECT_BINARY_DIR}/sources/version"
                        PUBLIC     "${Vulkan_INCLUDE_DIR}"
                        PRIVATE     "${CMAKE_CURRENT_SOURCE_DIR}"
                        INTERFACE   "${CMAKE_CURRENT_SOURCE_DIR}/.."
)

target_link_libraries(render PUBLIC Vulkan::Vulkan)
