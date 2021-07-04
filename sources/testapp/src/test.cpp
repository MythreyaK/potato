#include "test.hpp"

#include <chrono>
#include <filesystem>
#include <fstream>
#include <graphics/render.hpp>
#include <ios>
#include <vector>

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
        int count = 0;

        std::vector<testapp::vertex> vertices {
            { { 0.0f, -0.5f }, { 1.0f, 0.0f, 0.0f } },
            { { 0.5f, 0.5f }, { 0.0f, 1.0f, 0.0f } },
            { { -0.5f, 0.5f }, { 0.0f, 0.0f, 1.0f } },
        };

        vertex_model.emplace_back(m_renderer.get_device().shared_from_this(),
                                  vertices);

        while ( keep_window_open() ) {
            poll_events();

            while ( minimized ) {
                wait_events();
            }

            auto& cmd_buffer { m_renderer.get_swapchain().begin_frame() };

            m_renderer.get_swapchain().begin_renderpass();

            m_render_system.render_objects(cmd_buffer, vertex_model);

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

}  // namespace testapp
