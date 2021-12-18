#ifndef GLFW_HPP
#define GLFW_HPP

#include "glfwinclude.hpp"

#include <chrono>
#include <iostream>
#include <string>
#include <vector>

namespace glfw {

    struct icon {
        int                    width {};
        int                    height {};
        std::vector<std::byte> pixels {};
    };

    class window {
      private:
        static bool init_success;
        static bool init_complete;
        GLFWwindow* window_handle;
        bool        is_iconified { false };

        static bool init_glfw();

        static void _on_scroll(GLFWwindow*, double xoffset, double yoffset);
        static void _on_codepoint(GLFWwindow*, unsigned int codepoint);
        static void _on_window_resize(GLFWwindow*, int width, int height);
        static void _on_window_refresh(GLFWwindow*);
        static void _on_window_iconify(GLFWwindow*, int);

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
        bool is_minimized() const;
        void set_window_should_close();
        void set_title(const std::string& new_title);

        std::pair<int, int> framebuffer_size() const;

        bool set_icon(std::vector<icon>);

        virtual void window_loop();
        virtual void swap_buffers();
        virtual void on_scroll(double x, double y);
        virtual void on_codepoint(unsigned int codepoint);
        virtual void on_window_resized(int new_width, int new_height);
        virtual void on_window_refresh();
    };

    enum class im {
        CURSOR               = GLFW_CURSOR,
        STICKY_KEYS          = GLFW_STICKY_KEYS,
        STICKY_MOUSE_BUTTONS = GLFW_STICKY_MOUSE_BUTTONS,
        LOCK_KEY_MODS        = GLFW_LOCK_KEY_MODS,
        RAW_MOUSE_MOTION     = GLFW_RAW_MOUSE_MOTION,
    };

    enum class cm {
        NORMAL   = GLFW_CURSOR_NORMAL,
        HIDDEN   = GLFW_CURSOR_HIDDEN,
        DISABLED = GLFW_CURSOR_DISABLED,
    };

    enum class kaction {
        PRESS   = GLFW_PRESS,
        REPEAT  = GLFW_REPEAT,
        RELEASE = GLFW_RELEASE,
    };

    enum class kmod {
        SHIFT     = GLFW_MOD_SHIFT,
        CONTROL   = GLFW_MOD_CONTROL,
        ALT       = GLFW_MOD_ALT,
        SUPER     = GLFW_MOD_SUPER,
        CAPS_LOCK = GLFW_MOD_CAPS_LOCK,
        NUM_LOCK  = GLFW_MOD_NUM_LOCK,
    };
}  // namespace glfw

#endif  // !GLFW_HPP
