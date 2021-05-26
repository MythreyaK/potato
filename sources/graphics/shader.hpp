#ifndef RENDER_PIPELINE_SHADER_HPP
#define RENDER_PIPELINE_SHADER_HPP

#include "vkinclude/vulkan.hpp"

#include <vector>
#include <string>

namespace potato::render {

    std::vector<std::byte> load_shader(const std::string& fname);
}


#endif // !RENDER_PIPELINE_SHADER_HPP
