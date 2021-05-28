#ifndef POTATO_RENDER_HPP
#define POTATO_RENDER_HPP

// clang-format off
#include "vkinclude/vulkan.hpp"
// clang-format on
#include "context.hpp"
#include "device.hpp"
#include "pipeline.hpp"
#include "surface.hpp"

#include <vector>

namespace potato::render {

    class render_instance {
        context context;
        device  render_device;
        surface render_surface;

      public:
        render_instance(const std::vector<std::string> required_extensions,
                        GLFWwindow*                    window_handle);
    };

}  // namespace potato::render

#endif  // !POTATO_RENDER_HPP
