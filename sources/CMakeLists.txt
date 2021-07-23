cmake_minimum_required (VERSION 3.20)

add_compile_options("/experimental:module")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "$<TARGET_PROPERTY:CMAKE_CURRENT_BINARY_DIR>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "$<TARGET_PROPERTY:CMAKE_CURRENT_BINARY_DIR>")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "$<TARGET_PROPERTY:CMAKE_CURRENT_BINARY_DIR>")

add_subdirectory("modules")
add_subdirectory("potato")
add_subdirectory("glfwcpp")
add_subdirectory("testapp")
add_subdirectory("shaders")
