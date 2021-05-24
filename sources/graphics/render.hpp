#ifndef POTATO_RENDER_HPP
#define POTATO_RENDER_HPP

#include "context.hpp"
#include "device.hpp"
#include "vkinclude/vulkan.hpp"

#include <vector>

namespace potato::render {

    class render_instance {
        context render_context;
        device  render_device;

      public:
        render_instance(const std::vector<std::string> required_extensions,
                        GLFWwindow*                    window_handle)
          : render_context(window_handle)
          , render_device(render_context.get_instance(), window_handle) {}
    };

}  // namespace potato::render

#endif  // !POTATO_RENDER_HPP
