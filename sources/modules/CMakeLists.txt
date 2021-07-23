cmake_minimum_required (VERSION 3.20)

file(GLOB_RECURSE MODULE_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} CONFIGURE_DEPENDS *.mpp)

add_library(modules ${MODULE_FILES})

set_source_files_properties(${MODULE_FILES} PROPERTIES
                        LANGUAGE CXX
                        COMPILE_FLAGS "/interface"
)

target_include_directories(modules
                        PRIVATE "${Vulkan_INCLUDE_DIR}"
)

target_link_libraries(modules PRIVATE glfw)
