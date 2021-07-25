cmake_minimum_required (VERSION 3.20)

set(GLFW_CPP
    "glfw.cpp"
)

set(GLFW_MPP
    "glfw.mpp"
    "keys.mpp"
    "input.mpp"
)

message("fl: ${GLFW_MPP}")

add_library(glfwcpp STATIC "${GLFW_CPP}" "${GLFW_MPP}")

set_source_files_properties(${GLFW_MPP} PROPERTIES
                        LANGUAGE CXX
                        COMPILE_OPTIONS "/interface"
)

target_include_directories(glfwcpp
                        PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}"
                        PRIVATE "${Vulkan_INCLUDE_DIR}"
)

target_link_libraries(glfwcpp PRIVATE glfw)
