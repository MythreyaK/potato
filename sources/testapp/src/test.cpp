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
            { { -0.5f, 0.5f }, { 0.0f, 0.0f, 1.0f } }
        };

        std::vector<potato::model> vertex_model {};

        vertex_model.emplace_back(renderer.get_device(), vertices);

        while ( keep_window_open() ) {
            poll_events();
            renderer.render_objects(vertex_model);
        }

        renderer.get_device()->logical().waitIdle();
    }

    void testapp::run() {
        window_loop();
    }

    void testapp::on_window_resized(int new_width, int new_height) {
        renderer.window_resized();
    }

    std::vector<std::byte> read_icon(const std::string& fname) {

        namespace fs = std::filesystem;

        const auto     file_size { fs::file_size(fname) };
        constexpr auto FILE_FAIL_OPEN = static_cast<std::uintmax_t>(-1);

        if ( file_size != FILE_FAIL_OPEN ) {
            std::vector<std::byte> icon_buffer(file_size);

            std::ifstream file(fname, std::ios::binary);

            if ( !file.is_open() ) {
                throw std::runtime_error("Could not open file for reading");
            }

            file.read(reinterpret_cast<char*>(icon_buffer.data()),
                      icon_buffer.size());
            file.close();

            return icon_buffer;
        }

        else {
            throw std::runtime_error("Invalid file path");
        }
    }

}  // namespace Test
