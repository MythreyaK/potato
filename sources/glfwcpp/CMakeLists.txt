cmake_minimum_required (VERSION 3.16)

add_library(glfwcpp STATIC "glfw.cpp" "glfw.hpp")

target_include_directories(glfwcpp
                        INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/.."
)

target_link_libraries(glfwcpp PRIVATE glfw)
