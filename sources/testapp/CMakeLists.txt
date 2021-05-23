cmake_minimum_required (VERSION 3.18)

add_executable(testapp "test.cpp" "test.hpp")

target_link_libraries(testapp 
                        PUBLIC glfwcpp
                        PUBLIC render
)
