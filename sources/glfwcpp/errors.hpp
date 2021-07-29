#ifndef GLFW_ERRORS_HPP
#define GLFW_ERRORS_HPP

#include "glfwinclude.hpp"

#include <format>
#include <string>

namespace glfw {

    std::string to_string(int e) {
        switch ( e ) {
            case GLFW_API_UNAVAILABLE:
                return "API_UNAVAILABLE";
            case GLFW_FORMAT_UNAVAILABLE:
                return "FORMAT_UNAVAILABLE";
            case GLFW_INVALID_ENUM:
                return "INVALID_ENUM";
            case GLFW_INVALID_VALUE:
                return "INVALID_VALUE";
            case GLFW_NO_CURRENT_CONTEXT:
                return "NO_CURRENT_CONTEXT";
            case GLFW_NOT_INITIALIZED:
                return "NOT_INITIALIZED";
            case GLFW_OUT_OF_MEMORY:
                return "OUT_OF_MEMORY";
            case GLFW_PLATFORM_ERROR:
                return "PLATFORM_ERROR";
            case GLFW_VERSION_UNAVAILABLE:
                return "VERSION_UNAVAILABLE";
        }

        return "ERROR_UNKNOWN";
    }
}  // namespace glfw

namespace glfw {
    void check_error(int c_code) {
        if ( c_code != GLFW_NO_ERROR ) {
            const char** emess {};
            throw std::runtime_error(
              std::format("GLFW internal error {}", *emess));
        }
    }

    void check_error() {
        const char** emess {};
        int          c_code = glfwGetError(emess);

        if ( c_code != GLFW_NO_ERROR ) {
            const char** emess {};
            throw std::runtime_error(
              std::format("GLFW internal error {}", *emess));
        }
    }
}  // namespace glfw

#endif
