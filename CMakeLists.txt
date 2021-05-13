cmake_minimum_required (VERSION 3.18)

project ("Potato")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(Vulkan REQUIRED FATAL_ERROR)


# Include sub-projects.
set(SOURCES_SUB_DIR "${PROJECT_SOURCE_DIR}/sources")

# Dependencies
set(DEPS_DIR "${PROJECT_SOURCE_DIR}/third-party")

# Deps
add_subdirectory("${DEPS_DIR}")

add_subdirectory("${SOURCES_SUB_DIR}")
