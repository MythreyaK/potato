cmake_minimum_required (VERSION 3.20)

file(GLOB_RECURSE TESTAPP_HPP RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} CONFIGURE_DEPENDS *.hpp)
file(GLOB_RECURSE TESTAPP_CPP RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} CONFIGURE_DEPENDS *.cpp)

add_library(testapp STATIC ${TESTAPP_HPP} ${TESTAPP_CPP})

target_link_libraries(testapp
                        PUBLIC glfwcpp
                        PUBLIC glm::glm
                        PUBLIC render
)
