#include "device.hpp"
#include "utils.hpp"

#include <cstddef>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>

namespace potato::render {

    vk::ShaderModule device::create_shader(const std::string& fname) const {
        auto data { read_file(fname) };

        return logical_device->createShaderModule(vk::ShaderModuleCreateInfo {
          .codeSize = data.size(),
          .pCode    = reinterpret_cast<uint32_t*>(data.data()) });
    }

}  // namespace potato::render
