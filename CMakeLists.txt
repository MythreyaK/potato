cmake_minimum_required (VERSION 3.20)

project ("Potato")

set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

string(TOUPPER ${CMAKE_BUILD_TYPE} UC_CMAKE_BUILD_TYPE)

if (UC_CMAKE_BUILD_TYPE MATCHES "DEBUG")
    set(DEBUG_WINDOW TRUE)
    set(BUILD_TYPE "Debug")
    set(POTATO_DEBUG TRUE)
    message(STATUS "${PROJECT_NAME}: Debug window enabled")

    elseif(UC_CMAKE_BUILD_TYPE MATCHES "RELEASE")
    set(DEBUG_WINDOW FALSE)
    set(BUILD_TYPE "Release")

    elseif(UC_CMAKE_BUILD_TYPE MATCHES "RELWITHDEBINFO")
    set(DEBUG_WINDOW FALSE)
    set(BUILD_TYPE "ReleaseWithDebugInfo")
    set(POTATO_DEBUG TRUE)

endif()

# Include sub-projects.
set(SOURCES_SUB_DIR "${PROJECT_SOURCE_DIR}/sources")

# Dependencies
set(DEPS_DIR "${PROJECT_SOURCE_DIR}/third-party")

include("cmake/packages.cmake")

find_package(Vulkan REQUIRED FATAL_ERROR)
find_package(glm REQUIRED FATAL_ERROR)

# Deps
add_subdirectory("${DEPS_DIR}")

add_subdirectory("${SOURCES_SUB_DIR}")
