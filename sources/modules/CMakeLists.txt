cmake_minimum_required (VERSION 3.20)

file(GLOB_RECURSE HEADER_UNITS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} CONFIGURE_DEPENDS *.mpp)

add_library(modules ${HEADER_UNITS})

set_source_files_properties(${HEADER_UNITS} PROPERTIES
                        LANGUAGE CXX
                        COMPILE_FLAGS "/interface"
)

target_include_directories(modules
                        PRIVATE "${Vulkan_INCLUDE_DIR}"
)

target_link_libraries(modules PRIVATE glfw)

message("MODULES ::::: ${HEADER_UNITS}")
