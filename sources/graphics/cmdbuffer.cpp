#include "render.hpp"

namespace potato::render {

    void render_instance::create_command_buffers(uint32_t graphics_queue) {
        using cmdci = vk::CommandPoolCreateInfo;

        const cmdci cmdpool_ci { .queueFamilyIndex = graphics_queue };

        cmdpool = render_device.logical().createCommandPool(cmdpool_ci);

        const vk::CommandBufferAllocateInfo cmd_alloc_ci {
            .commandPool        = cmdpool,
            .level              = vk::CommandBufferLevel::ePrimary,
            .commandBufferCount = 2,
        };

        commandbuffers = std::move(
          render_device.logical().allocateCommandBuffers(cmd_alloc_ci));
    }

    void render_instance::destroy_command_buffers() {
        render_device.logical().freeCommandBuffers(cmdpool, commandbuffers);
        render_device.logical().destroyCommandPool(cmdpool);
    }

}  // namespace potato::render
