cmake_minimum_required (VERSION 3.18)

add_subdirectory("graphics")
add_subdirectory("glfwcpp")
add_subdirectory("testapp")
add_subdirectory("shaders")

configure_file("version.hpp.in" "version/version.hpp" @ONLY NEWLINE_STYLE LF)
