cmake_minimum_required (VERSION 3.22)

file(GLOB_RECURSE GLFW_HPP RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} CONFIGURE_DEPENDS *.hpp)
file(GLOB_RECURSE GLFW_CPP RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} CONFIGURE_DEPENDS *.cpp)

add_library(glfwcpp STATIC ${GLFW_CPP} ${GLFW_HPP})

target_include_directories(glfwcpp
                        INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/.."
                        PUBLIC "${Vulkan_INCLUDE_DIR}"
)

target_link_libraries(glfwcpp PUBLIC glfw)
