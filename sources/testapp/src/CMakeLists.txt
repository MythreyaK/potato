cmake_minimum_required (VERSION 3.22)

file(GLOB_RECURSE TESTAPP_HPP RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} CONFIGURE_DEPENDS *.hpp)
file(GLOB_RECURSE TESTAPP_CPP RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} CONFIGURE_DEPENDS *.cpp)

add_library(testapp STATIC ${TESTAPP_HPP} ${TESTAPP_CPP})

target_link_libraries(testapp
                        PUBLIC glfwcpp
                        PUBLIC potato_lib
                        PUBLIC pch
)

add_dependencies(testapp shaders)

target_precompile_headers(testapp REUSE_FROM pch)
