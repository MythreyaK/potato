#include "test.hpp"

#include <chrono>
#include <filesystem>
#include <fstream>
#include <graphics/render.hpp>
#include <ios>
#include <vector>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "render/camera.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

namespace testapp {
    app::app(int                     width,
             int                     height,
             const std::string&      title,
             std::vector<glfw::icon> icons)
      : glfw::window { width, height, title, icons }
      , m_renderer { get_handle() }
      , m_render_system { m_renderer.get_device().logical.get(),
                          m_renderer.get_swapchain().get_renderpass() } {}

    void app::window_loop() {
        int    count = 0;
        camera camera {};
        camera.setViewTarget(glm::vec3(-1.f, -2.f, -2.f),
                             glm::vec3(0.f, 0.f, 2.5f));

        create_cube_model({ .0f, .0f, .0f });

        while ( keep_window_open() ) {
            poll_events();

            while ( minimized ) {
                wait_events();
            }

            camera.setPerspectiveProjection(
              glm::radians(50.f),
              m_renderer.get_swapchain().get_aspect(),
              0.1f,
              10.f);

            auto& cmd_buffer { m_renderer.get_swapchain().begin_frame() };

            m_renderer.get_swapchain().begin_renderpass();

            for ( auto& obj : vertex_model ) {
                obj.transform.rotation.y =
                  glm::mod(obj.transform.rotation.y + 0.01f,
                           glm::two_pi<float>());

                obj.transform.rotation.x =
                  glm::mod(obj.transform.rotation.x + 0.005f,
                           glm::two_pi<float>());
            }

            m_render_system.render_objects(cmd_buffer, vertex_model, camera);

            m_renderer.get_swapchain().end_renderpass();
            m_renderer.get_swapchain().end_frame();
        }
    }

    void app::run() {
        window_loop();
        m_renderer.get_device().logical->waitIdle();
    }

    void app::on_window_resized(int new_width, int new_height) {
        // TODO: Maybe suspend all processing when minimized?
        minimized = is_minimized() || (new_width == 0) || (new_height == 0);

        if ( !minimized ) {
            m_renderer.window_resized();
        }
    }

    void app::create_cube_model(glm::vec3 offset) {

        testapp::mesh vertices {

            // left face (white)
            { { -.5f, -.5f, -.5f }, { .9f, .9f, .9f } },
            { { -.5f, .5f, .5f }, { .9f, .9f, .9f } },
            { { -.5f, -.5f, .5f }, { .9f, .9f, .9f } },
            { { -.5f, -.5f, -.5f }, { .9f, .9f, .9f } },
            { { -.5f, .5f, -.5f }, { .9f, .9f, .9f } },
            { { -.5f, .5f, .5f }, { .9f, .9f, .9f } },

            // right face (yellow)
            { { .5f, -.5f, -.5f }, { .8f, .8f, .1f } },
            { { .5f, .5f, .5f }, { .8f, .8f, .1f } },
            { { .5f, -.5f, .5f }, { .8f, .8f, .1f } },
            { { .5f, -.5f, -.5f }, { .8f, .8f, .1f } },
            { { .5f, .5f, -.5f }, { .8f, .8f, .1f } },
            { { .5f, .5f, .5f }, { .8f, .8f, .1f } },

            // top face (orange, remember y axis points down)
            { { -.5f, -.5f, -.5f }, { .9f, .6f, .1f } },
            { { .5f, -.5f, .5f }, { .9f, .6f, .1f } },
            { { -.5f, -.5f, .5f }, { .9f, .6f, .1f } },
            { { -.5f, -.5f, -.5f }, { .9f, .6f, .1f } },
            { { .5f, -.5f, -.5f }, { .9f, .6f, .1f } },
            { { .5f, -.5f, .5f }, { .9f, .6f, .1f } },

            // bottom face (red)
            { { -.5f, .5f, -.5f }, { .8f, .1f, .1f } },
            { { .5f, .5f, .5f }, { .8f, .1f, .1f } },
            { { -.5f, .5f, .5f }, { .8f, .1f, .1f } },
            { { -.5f, .5f, -.5f }, { .8f, .1f, .1f } },
            { { .5f, .5f, -.5f }, { .8f, .1f, .1f } },
            { { .5f, .5f, .5f }, { .8f, .1f, .1f } },

            // nose face (blue)
            { { -.5f, -.5f, 0.5f }, { .1f, .1f, .8f } },
            { { .5f, .5f, 0.5f }, { .1f, .1f, .8f } },
            { { -.5f, .5f, 0.5f }, { .1f, .1f, .8f } },
            { { -.5f, -.5f, 0.5f }, { .1f, .1f, .8f } },
            { { .5f, -.5f, 0.5f }, { .1f, .1f, .8f } },
            { { .5f, .5f, 0.5f }, { .1f, .1f, .8f } },

            // tail face (green)
            { { -.5f, -.5f, -0.5f }, { .1f, .8f, .1f } },
            { { .5f, .5f, -0.5f }, { .1f, .8f, .1f } },
            { { -.5f, .5f, -0.5f }, { .1f, .8f, .1f } },
            { { -.5f, -.5f, -0.5f }, { .1f, .8f, .1f } },
            { { .5f, -.5f, -0.5f }, { .1f, .8f, .1f } },
            { { .5f, .5f, -0.5f }, { .1f, .8f, .1f } },

        };

        for ( auto& v : vertices ) {
            v.position += offset;
        }

        vertex_model.emplace_back(m_renderer.get_device().shared_from_this(),
                                  vertices);

        vertex_model[0].transform.translation = { .0f, .0f, 2.5f };
        vertex_model[0].transform.scale       = { 1.5f, 1.5f, 1.5f };
    }

}  // namespace testapp
