#ifndef GLFW_CPP
#define GLFW_CPP

#include "GLFW.hpp"

// clang-format off
// Vulkan needs to be included before GLFW
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
// clang-format on

namespace glfw {

    bool window::init_glfw(int w, int h, const std::string& title) {
        /* Initialize the library */
        if ( !glfwInit() ) {
            return false;
        }

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        GLFWwindow* window = glfwCreateWindow(w, h, title.c_str(), NULL, NULL);

        if ( !window ) {
            return false;
        }

        window_handle = window;
        glfwSetWindowUserPointer(window, this);
        return true;
    }

    window::window(int w, int h, const std::string& title) try
      : window_handle(nullptr)
    {
        if ( !init_glfw(w, h, title) )
            throw std::runtime_error("GLFW initialization Error");

        // hook up callbacks
        glfwSetCharCallback(window_handle, window::_on_codepoint);
        glfwSetScrollCallback(window_handle, window::_on_scroll);
        glfwSetFramebufferSizeCallback(window_handle,
                                       window::_on_window_resize);
    }
    catch ( const std::exception& ) {
    }

    window::~window() {
        glfwTerminate();
    }

    // Other functions

    std::vector<std::string> window::required_extns() const {
        auto glfwExtCount { 0u };
        auto glfwExt { glfwGetRequiredInstanceExtensions(&glfwExtCount) };
        return { glfwExt, glfwExt + glfwExtCount };
    }

    VkSurfaceKHR window::create_surface(const VkInstance& instance) const {
        VkSurfaceKHR surface {};
        if ( glfwCreateWindowSurface(instance, window_handle, nullptr, &surface)
             == VK_SUCCESS )
        {
            return surface;
        }
        else {
            return nullptr;
        }
    }

    GLFWwindow* window::get_handle() {
        return window_handle;
    }

    bool window::keep_window_open() const {
        return !static_cast<bool>(glfwWindowShouldClose(window_handle));
    }

    void window::poll_events() const {
        glfwPollEvents();
    }

    void window::wait_events() const {
        glfwWaitEvents();
    }

    void window::wait_events(std::chrono::milliseconds t) const {
        // Takes seconds
        glfwWaitEventsTimeout(t.count() / 1000);
    }

    void window::set_window_should_close() {
        glfwSetWindowShouldClose(window_handle, 1);
    }

    void window::set_title(const std::string& new_title) {
        glfwSetWindowTitle(window_handle, new_title.c_str());
    }

    void window::window_loop() {};
    void window::swap_buffers() {};
    void window::on_scroll(double x, double y) {}
    void window::on_codepoint(unsigned int codepoint) {}
    void window::on_window_resized(int new_width, int new_height) {}

    // ** C to C++ functions callbacks mapping **

    void window::_on_scroll(GLFWwindow* wd, double xoffset, double yoffset) {
        get_instance(wd)->on_scroll(xoffset, yoffset);
    }

    void window::_on_codepoint(GLFWwindow* wd, unsigned int codepoint) {
        get_instance(wd)->on_codepoint(codepoint);
    }

    void window::_on_window_resize(GLFWwindow* wd, int w, int h) {
        get_instance(wd)->on_window_resized(w, h);
    }

    window* window::get_instance(GLFWwindow* handle) {
        return reinterpret_cast<window*>(glfwGetWindowUserPointer(handle));
    }

}  // namespace glfw

#endif  // GLFW_CPP
