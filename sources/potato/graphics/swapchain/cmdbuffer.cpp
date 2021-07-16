#include "device/device.hpp"
#include "swapchain.hpp"

namespace potato::graphics {

    void swapchain::create_command_buffers(uint32_t graphics_queue) {
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

        m_cmd_pool = m_device->logical->createCommandPool(cmd_pool_ci);

        const vk::CommandBufferAllocateInfo cmd_alloc_ci {
            .commandPool        = m_cmd_pool,
            .level              = vk::CommandBufferLevel::ePrimary,
            .commandBufferCount = MAX_FRAMES_IN_FLIGHT,
        };

        m_cmd_buffers =
          std::move(m_device->logical->allocateCommandBuffers(cmd_alloc_ci));
    }

    void swapchain::destroy_command_buffers() {
        m_device->logical->freeCommandBuffers(m_cmd_pool, m_cmd_buffers);
        m_device->logical->destroyCommandPool(m_cmd_pool);
    }

    const vk::CommandBuffer& swapchain::current_cmd_buffer() const {
        return m_cmd_buffers[m_current_frame];
    }

}  // namespace potato::graphics
