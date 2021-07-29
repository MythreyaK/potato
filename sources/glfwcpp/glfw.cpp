module;

#include "glfw_include.hpp"

module glfw;

import : errors;

namespace glfw {

    bool window::init_success = false;
    bool window::init_complete = false;

    bool window::init_glfw() {
        init_success = glfwInit();
        if ( !init_success ) {
            check_error();
        }
        init_complete = true;
        return init_success;
    }

    window::window(int                w,
                   int                h,
                   const std::string& title,
                   std::vector<icon>  icons) try
      : window_handle(nullptr)
    {
        if (!init_complete) init_glfw();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        GLFWwindow* window = glfwCreateWindow(w, h, title.c_str(), NULL, NULL);

        if ( !window ) {
            check_error();
        }

        window_handle = window;
        glfwSetWindowUserPointer(window, this);

        // hook up callbacks
        // glfwSetCharCallback(window_handle, window::_on_codepoint);
        // glfwSetScrollCallback(window_handle, window::_on_scroll);
        glfwSetFramebufferSizeCallback(window_handle, window::_on_window_resize);
        glfwSetWindowIconifyCallback(window_handle, window::_on_window_iconify);
        glfwSetWindowRefreshCallback(window_handle, _on_window_refresh);
        set_icon(icons);
    }
    catch ( const std::exception& ) {
    }

    static void terminate() {
        glfwTerminate();
    }

    window::~window() {
        glfwDestroyWindow(window_handle);
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

    bool window::set_icon(std::vector<icon> images) {
        std::vector<GLFWimage> _icons {};
        for ( auto& icon : images ) {
            _icons.emplace_back(GLFWimage {
              .width  = icon.width,
              .height = icon.height,
              .pixels = reinterpret_cast<unsigned char*>(icon.pixels.data()) });
        }
        glfwSetWindowIcon(window_handle, _icons.size(), _icons.data());
        if ( glfwGetError(NULL) == GLFW_PLATFORM_ERROR ) {
            // could not set, return false
            return false;
        }
        else {
            return true;
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

    void window::set_window_position(int x, int y) const {
        glfwSetWindowPos(window_handle, x, y);
    }

    std::pair<int, int> window::get_window_position() const {
        int x, y;
        glfwGetWindowPos(window_handle, &x, &y);
        return { x, y };
    }

    bool window::is_minimized() const {
        return is_iconified;
    }

    std::pair<int, int> window::framebuffer_size() const {
        int width, height;
        glfwGetFramebufferSize(window_handle, &width, &height);
        return { width, height };
    }

    std::pair<double, double> window::get_cursor_position() const {
        double x, y;
        glfwGetCursorPos(window_handle, &x, &y);
        return { x, y };
    }

    void window::set_cursor_position(double x, double y) const {
        glfwSetCursorPos(window_handle, x, y);
    }

    void window::set_cursor_mode(cm a) const {
        glfwSetInputMode(window_handle, GLFW_CURSOR, static_cast<int>(a));
    }

    void window::set_input_mode(im a, bool b) const {
        glfwSetInputMode(window_handle,
                         static_cast<int>(a),
                         static_cast<int>(b));
    }

    void window::window_loop() {};
    void window::swap_buffers() {};
    void window::on_scroll(double x, double y) {}
    void window::on_codepoint(unsigned int codepoint) {}
    void window::on_window_resized(int new_width, int new_height) {}
    void window::on_window_refresh() {}

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

    void window::_on_window_iconify(GLFWwindow* wd, int iconify) {
        get_instance(wd)->is_iconified = iconify == GLFW_TRUE ? true : false;
    }

    void window::_on_window_refresh(GLFWwindow* wd) {
        get_instance(wd)->on_window_refresh();
    }

    window* window::get_instance(GLFWwindow* handle) {
        return reinterpret_cast<window*>(glfwGetWindowUserPointer(handle));
    }

}  // namespace glfw
