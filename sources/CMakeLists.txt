cmake_minimum_required (VERSION 3.20)

add_compile_options("/experimental:module")

add_subdirectory("modules")
add_subdirectory("potato")
add_subdirectory("glfwcpp")
add_subdirectory("testapp")
add_subdirectory("shaders")
