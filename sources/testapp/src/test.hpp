#ifndef TESTAPP
#define TESTAPP

#include "render/render.hpp"

#include <core/time.hpp>
#include <ecs/ecs.hpp>
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
        potato::chrono::timer m_timer { "Main loop" };
        bool                  minimized { false };
        ecs::context          ecs_context {};

      public:
        app(int                width,
            int                height,
            const std::string& title,
            std::vector<glfw::icon>);

        void run();
        void window_loop() override;
        void on_window_resized(int new_width, int new_height) override;
        void create_cube_model(ecs::entity&, glm::vec3 offset);
        vk::RenderPass create_renderpass();
    };

}  // namespace testapp

#endif  // !TESTAPP
