#ifndef GLFW_HPP
#define GLFW_HPP

#include <chrono>
#include <iostream>
#include <string>
#include <vector>

extern "C" {
    struct GLFWwindow;
}

typedef GLFWwindow glfw_handle;

namespace glfw {

    class window {
      private:
        glfw_handle* window_handle;

        bool init_glfw(int w, int h, const std::string& title);

        static void _on_scroll(glfw_handle* window, double xoffset, double yoffset);
        static void _on_codepoint(glfw_handle* window, unsigned int codepoint);
        static void _on_window_resize(glfw_handle* w, int width, int height);

        static window* get_instance(glfw_handle* ptr);

      public:
        window(int w = 300, int h = 300, const std::string& title = std::string("Window"));
        window(window&)  = delete;
        window(window&&) = default;
        ~window();

        std::vector<const char*> required_extns();

        window& operator=(const window&) = delete;
        window& operator=(window&&) = default;

        void poll_events();
        void wait_events();
        void wait_events(std::chrono::milliseconds t);
        bool keep_window_open();
        void set_window_should_close();
        void set_title(const std::string& new_title);

        virtual void window_loop();
        virtual void swap_buffers();
        virtual void on_scroll(double x, double y);
        virtual void on_codepoint(unsigned int codepoint);
        virtual void on_window_resized(int new_width, int new_height);
    };
}  // namespace glfw

#endif  // !GLFW_HPP
