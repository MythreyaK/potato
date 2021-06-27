#ifndef TESTAPP
#define TESTAPP

#include "render/render.hpp"

#include <glfwcpp/glfw.hpp>
#include <graphics/render.hpp>
#include <iostream>
#include <string>

namespace pgfx = potato::graphics;

namespace testapp {

    class app : public glfw::window {
      private:
        pgfx::render_instance m_renderer;
        render_system         m_render_system;
        bool                  minimized { false };

        std::vector<testapp::model> vertex_model {};

      public:
        app(int                width,
            int                height,
            const std::string& title,
            std::vector<glfw::icon>);

        void run();
        void window_loop() override;
        void on_window_resized(int new_width, int new_height) override;
        vk::RenderPass create_renderpass();
    };

}  // namespace testapp

#endif  // !TESTAPP
