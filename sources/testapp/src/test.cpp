#include "test.hpp"

#include "render/camera.hpp"

#include <chrono>
#include <format>
#include <graphics/render.hpp>
#include <random>
#include <vector>

namespace {
    std::random_device                 rd;
    std::mt19937                       gen(rd());
    std::uniform_int_distribution<int> dist(-800, 800);  // distribution in range [1, 6]

    float get_rand() {
        return dist(gen);
    }

    void debug_glm(const glm::vec3& a) {
        std::cout << std::format("({}, {}, {})", a.x, a.y, a.z) << '\n';
    }
}  // namespace

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
        constexpr float rate { 2.0f * 0.001f };

        camera camera {};

        camera.setViewTarget(glm::vec3(0.f, -70.0f, -70.f),
                             glm::vec3(0.f, 0.0f, 0.0f));

        glm::vec3 rotations = { 0.0f, 0.0f, 0.0f };
        ecs_context.add_component<transform>();
        ecs_context.add_component<model>();
        std::vector<ecs::entity> entities {};

        for ( int i = 0; i < 1200; ++i ) {
            auto cube = ecs_context.create_entity();

            cube.add_component<transform>().translation = { get_rand() / 10, 0.0f, get_rand() / 10 };
            create_cube_model(cube, {});

            entities.emplace_back(std::move(cube));
        }

        while ( keep_window_open() ) {
            poll_events();

            while ( minimized ) {
                wait_events();
            }

            camera.setPerspectiveProjection(
              glm::radians(70.f),
              m_renderer.get_swapchain().get_aspect(),
              0.1f,
              250.f);

            auto& cmd_buffer { m_renderer.get_swapchain().begin_frame() };

            m_renderer.get_swapchain().begin_renderpass();

            auto advance { rate * m_timer.elapsed().count() };

            rotations.x += advance ;
            rotations.z += advance ;

            for ( auto& obj : entities ) {
                obj.get_component<transform>().euler_rotate(rotations);
            }

            m_render_system.render_objects(cmd_buffer, entities, camera);

            m_renderer.get_swapchain().end_renderpass();
            m_renderer.get_swapchain().end_frame();
        }

        m_renderer.get_device().logical->waitIdle();
    }

    void app::run() {
        window_loop();
    }

    void app::on_window_resized(int new_width, int new_height) {
        // TODO: Maybe suspend all processing when minimized?
        minimized = is_minimized() || (new_width == 0) || (new_height == 0);

        if ( !minimized ) {
            m_renderer.window_resized();
        }
    }

    void app::create_cube_model(ecs::entity& e, glm::vec3 offset) {

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

        // for ( auto& v : vertices ) {
        //     v.position += offset;
        // }

        e.add_component<model>(m_renderer.get_device().shared_from_this(),
                               vertices);

        // vertex_model.emplace_back(m_renderer.get_device().shared_from_this(),
        //                           vertices);

        // vertex_model[0].transform.translation = { .0f, .0f, 2.5f };
        // vertex_model[0].transform.scale       = { 1.5f, 1.5f, 1.5f };
        // e.get_component<transform>().translation = offset;
    }

}  // namespace testapp
