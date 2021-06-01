#ifndef POTATO_RENDER_HPP
#define POTATO_RENDER_HPP

// clang-format off
#include "vkinclude/vulkan.hpp"
// clang-format on
#include "context.hpp"
#include "device.hpp"
#include "surface.hpp"

#include <vector>

namespace potato::render {

    class render_instance {
      private:
        using vkcmdbuffers = std::vector<vk::CommandBuffer>;

        GLFWwindow*     window_handle;
        context         context;
        device          render_device;
        surface         render_surface;
        vk::CommandPool cmdpool {};
        vkcmdbuffers    commandbuffers {};

        void create_command_buffers(uint32_t graphics_queue);
        void destroy_command_buffers();

      public:
        render_instance(GLFWwindow* window_handle);
        void window_resized();

        // no copies
        render_instance(const render_instance&) = delete;
        render_instance& operator=(const render_instance&) = delete;

        // allow move
        render_instance(render_instance&&) = default;
        render_instance& operator=(render_instance&&) = default;
    };

}  // namespace potato::render

#endif  // !POTATO_RENDER_HPP
