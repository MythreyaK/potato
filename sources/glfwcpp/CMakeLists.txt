cmake_minimum_required (VERSION 3.20)

add_library(glfwcpp STATIC "glfw.cpp" "glfw.mpp")

set_source_files_properties("glfw.mpp" PROPERTIES
                        LANGUAGE CXX
                        COMPILE_FLAGS "/interface"
)

target_include_directories(glfwcpp
                        INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/.."
                        PRIVATE "${Vulkan_INCLUDE_DIR}"
)

target_link_libraries(glfwcpp PRIVATE glfw)
