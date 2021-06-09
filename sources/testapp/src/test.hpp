#ifndef TESTAPP
#define TESTAPP

#include "render/render.hpp"

#include <glfwcpp/glfw.hpp>
#include <iostream>
#include <string>

namespace potato_render = potato::render;

namespace testpotato {
    class testapp : public glfw::window {
      private:
        testpotato::render renderer;

      public:
        testapp(int                     width,
                int                     height,
                const std::string&      title,
                std::vector<glfw::icon> icons)
          : glfw::window(width, height, title, icons)
          , renderer(get_handle()) {}

        void run();
        void window_loop() override;
        void on_window_resized(int new_width, int new_height) override;
        vk::RenderPass create_renderpass();
    };

    std::vector<std::byte> read_icon(const std::string& fname);

}  // namespace testpotato

#endif  // !TESTAPP