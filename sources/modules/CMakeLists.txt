cmake_minimum_required (VERSION 3.20)

file(GLOB_RECURSE MODULE_FILES_MPP RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} CONFIGURE_DEPENDS *.mpp)
file(GLOB_RECURSE MODULE_FILES_IMP RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} CONFIGURE_DEPENDS *.imp)
file(GLOB_RECURSE MODULE_FILES_HPP RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} CONFIGURE_DEPENDS *.hpp)

add_library(modules ${MODULE_FILES_MPP} ${MODULE_FILES_HPP})

set_source_files_properties(${MODULE_FILES_MPP} PROPERTIES
                        LANGUAGE CXX
                        COMPILE_FLAGS "/interface"
)

set_source_files_properties(${MODULE_FILES_IMP} PROPERTIES
                        LANGUAGE CXX
                        COMPILE_FLAGS "/exportHeader"
)

target_include_directories(modules
                        PRIVATE "${Vulkan_INCLUDE_DIR}"
                        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_link_libraries(modules PUBLIC glm::glm)
