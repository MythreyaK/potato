#include "test.hpp"

#include <chrono>
#include <filesystem>
#include <fstream>
#include <graphics/render.hpp>
#include <ios>
#include <vector>

namespace testpotato {
    void testapp::window_loop() {
        int count = 0;

        std::vector<potato::vertex> vertices {
            { { 0.0f, -0.5f }, { 1.0f, 0.0f, 0.0f } },
            { { 0.5f, 0.5f }, { 0.0f, 1.0f, 0.0f } },
            { { -0.5f, 0.5f }, { 0.0f, 0.0f, 1.0f } },
        };

        vertex_model.emplace_back(renderer.get_device(), vertices);

        while ( keep_window_open() ) {
            poll_events();

            while ( minimized ) {
                wait_events();
            }

            renderer.render_objects(vertex_model);
        }

        renderer.get_device()->logical->waitIdle();
    }

    void testapp::run() {
        window_loop();
    }

    void testapp::on_window_resized(int new_width, int new_height) {
        // TODO: Maybe suspend all processing when minimized?
        minimized = is_minimized() || (new_width == 0) || (new_height == 0);

        if ( !minimized ) {
            renderer.window_resized();
        }
    }

}  // namespace testpotato
