#include "device.hpp"
#include "surface.hpp"

namespace potato::render {

    void surface::create_command_buffers(uint32_t graphics_queue) {
        using cmdci = vk::CommandPoolCreateInfo;

        constexpr auto cmdpool_flags {
            vk::CommandPoolCreateFlagBits::eResetCommandBuffer
            | vk::CommandPoolCreateFlagBits::eTransient
        };

        // clang-format off
        const cmdci cmd_pool_ci {
            .flags            = cmdpool_flags,
            .queueFamilyIndex = graphics_queue
        };
        // clang-format on

        cmd_pool = potato_device->logical().createCommandPool(cmd_pool_ci);

        const vk::CommandBufferAllocateInfo cmd_alloc_ci {
            .commandPool        = cmd_pool,
            .level              = vk::CommandBufferLevel::ePrimary,
            .commandBufferCount = MAX_FRAMES_IN_FLIGHT,
        };

        cmd_buffers = std::move(
          potato_device->logical().allocateCommandBuffers(cmd_alloc_ci));
    }

    const vk::CommandBuffer& surface::current_cmd_buffer() const {
        return cmd_buffers[current_frame];
    }

}  // namespace potato::render
