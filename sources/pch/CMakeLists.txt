cmake_minimum_required (VERSION 3.20)

file(GLOB_RECURSE POTATO_PCH_HPP RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} CONFIGURE_DEPENDS *.hpp)
file(GLOB_RECURSE POTATO_PCH_CPP RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} CONFIGURE_DEPENDS *.cpp)

add_library(pch STATIC ${POTATO_PCH_HPP} ${POTATO_PCH_CPP})

target_include_directories(pch
                    PUBLIC      "${CMAKE_CURRENT_SOURCE_DIR}"
                    PUBLIC      "${Vulkan_INCLUDE_DIR}"
)

target_link_libraries(pch
                    PUBLIC glm::glm
)

target_precompile_headers(pch PUBLIC ${POTATO_PCH_HPP})
