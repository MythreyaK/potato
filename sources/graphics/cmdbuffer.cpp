#include "render.hpp"

namespace potato::render {

    void render_instance::create_command_buffers(uint32_t graphics_queue) {
        using cmdci = vk::CommandPoolCreateInfo;

        const cmdci cmd_pool_ci { .queueFamilyIndex = graphics_queue };

        cmd_pool = render_device->logical().createCommandPool(cmd_pool_ci);

        const vk::CommandBufferAllocateInfo cmd_alloc_ci {
            .commandPool        = cmd_pool,
            .level              = vk::CommandBufferLevel::ePrimary,
            .commandBufferCount = 1,
        };

        cmd_buffers = std::move(
          render_device->logical().allocateCommandBuffers(cmd_alloc_ci));
    }

    void render_instance::destroy_command_buffers() {
        render_device->logical().freeCommandBuffers(cmd_pool, cmd_buffers);
        render_device->logical().destroyCommandPool(cmd_pool);
    }

}  // namespace potato::render
