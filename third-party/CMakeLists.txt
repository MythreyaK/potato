cmake_minimum_required (VERSION 3.22)

# Dependencies
# Needed?
set(GLFW_INSTALL        OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS     OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLM_TEST_ENABLE     OFF CACHE BOOL "" FORCE)

add_subdirectory("glfw")
add_subdirectory("glm")
