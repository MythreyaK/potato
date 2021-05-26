#include "shader.hpp"

#include <fstream>
#include <filesystem>
#include <iostream>
#include <vector>
#include <cstddef>

namespace potato::render {

    std::vector<std::byte> load_shader(const std::string& fname) {

        namespace fs = std::filesystem;

        const auto file_size{ fs::file_size(fname) };
        constexpr auto FILE_FAIL_OPEN = static_cast<std::uintmax_t>(-1);

        if (file_size != FILE_FAIL_OPEN) {
            std::vector<std::byte> shader_buffer(file_size);

            std::ifstream file(fname, std::ios::binary);

            if (!file.is_open()) {
                throw std::runtime_error("Could not open file");
            }

            file.read(reinterpret_cast<char*>(shader_buffer.data()), shader_buffer.size());
            file.close();
            
            return shader_buffer;
        }

        else {
            throw std::runtime_error("Invalid file path");
        }
    }

}
