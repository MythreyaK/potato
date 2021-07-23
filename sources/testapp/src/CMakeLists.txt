cmake_minimum_required (VERSION 3.20)

file(GLOB_RECURSE TESTAPP_MPP RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} CONFIGURE_DEPENDS *.mpp)
file(GLOB_RECURSE TESTAPP_CPP RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} CONFIGURE_DEPENDS *.cpp)

add_library(testapp STATIC ${TESTAPP_MPP} ${TESTAPP_CPP})

set_source_files_properties(${TESTAPP_MPP} PROPERTIES
                        LANGUAGE CXX
                        COMPILE_FLAGS "/interface"
)

target_link_libraries(testapp
                        PUBLIC glfwcpp
                        PUBLIC glm::glm
                        PUBLIC potato_lib
)

add_dependencies(testapp shaders)
# target_precompile_headers(testapp REUSE_FROM potato_lib)
