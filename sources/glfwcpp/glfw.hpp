#ifndef GLFW_HPP
#define GLFW_HPP

#include <chrono>
#include <iostream>
#include <string>
#include <vector>

extern "C" {
    struct GLFWwindow;
    struct VkSurfaceKHR_T;
    struct VkInstance_T;
    typedef VkInstance_T*   VkInstance;
    typedef VkSurfaceKHR_T* VkSurfaceKHR;
}

namespace glfw {

    struct icon {
        int                    width {};
        int                    height {};
        std::vector<std::byte> pixels {};
    };

    class window {
      private:
        GLFWwindow* window_handle;

        bool init_glfw(int w, int h, const std::string& title);

        static void _on_scroll(GLFWwindow* window,
                               double      xoffset,
                               double      yoffset);
        static void _on_codepoint(GLFWwindow* window, unsigned int codepoint);
        static void _on_window_resize(GLFWwindow* w, int width, int height);

        static window* get_instance(GLFWwindow* ptr);

      public:
        window(int                w     = 300,
               int                h     = 300,
               const std::string& title = std::string("Window"),
               std::vector<icon>  icons = {});
        window(window&)  = delete;
        window(window&&) = default;
        ~window();

        std::vector<std::string> required_extns() const;

        window& operator=(const window&) = delete;
        window& operator=(window&&) = default;

        VkSurfaceKHR create_surface(const VkInstance&) const;
        GLFWwindow*  get_handle();

        void poll_events() const;
        void wait_events() const;
        void wait_events(std::chrono::milliseconds t) const;
        bool keep_window_open() const;
        void set_window_should_close();
        void set_title(const std::string& new_title);

        bool set_icon(std::vector<icon>);

        virtual void window_loop();
        virtual void swap_buffers();
        virtual void on_scroll(double x, double y);
        virtual void on_codepoint(unsigned int codepoint);
        virtual void on_window_resized(int new_width, int new_height);
    };
}  // namespace glfw

#endif  // !GLFW_HPP
