#ifndef POTATO_RENDER_SWAPCHAIN_HPP
#define POTATO_RENDER_SWAPCHAIN_HPP

#include "device/create_info.hpp"
#include "memory/memory.hpp"
#include "pipeline.hpp"

#include <unordered_map>
// #include "vkinclude/vulkan.hpp"

extern "C" {
    struct GLFWwindow;
}

namespace potato::graphics {
    class device;
    class surface;

    class swapchain {
        using vkimages       = std::vector<vk::Image>;
        using vkimageviews   = std::vector<vk::ImageView>;
        using vkframebuffers = std::vector<vk::Framebuffer>;
        using vkdepthmemory  = std::vector<vk::DeviceMemory>;
        using vkcmdbuffers   = std::vector<vk::CommandBuffer>;
        using vksemaphores   = std::vector<vk::Semaphore>;
        using vkfences       = std::vector<vk::Fence>;

      private:
        std::shared_ptr<device>        m_device {};
        device_create_info             m_device_info {};
        std::shared_ptr<const surface> m_surface {};
        vk::SwapchainKHR               m_swapchain {};
        uint32_t                       MAX_FRAMES_IN_FLIGHT {};
        vkimages                       m_swapimages {};
        vkimageviews                   m_swapimageviews {};
        vkdepthmemory                  m_depthimagesmemory {};
        vkimages                       m_depthimages {};
        vkimageviews                   m_depthimageviews {};
        vk::CommandPool                m_cmd_pool {};
        vkcmdbuffers                   m_cmd_buffers {};
        vk::RenderPass                 m_renderpass {};
        vkframebuffers                 m_framebuffers {};

        // pipeline waits for m_image_available before write
        vksemaphores m_image_available {};
        vksemaphores m_render_complete {};
        vkfences     m_in_flight_fence {};
        vkfences     m_in_flight_image {};
        uint32_t     m_current_frame { 0 };
        uint32_t     m_framebuffer_inx { 0 };
        bool         m_frame_in_progress { false };

        // methods
        void create_swapchain();
        void create_depth_resources();
        void create_swapchain_images();
        void create_renderpass();
        void create_sync_objects();
        void create_framebuffers();

        void destroy_swapchain_images();
        void destroy_framebuffers();
        void destroy_renderpass();
        void destroy_sync_objects();
        void destroy_command_buffers();
        void acquire_image();

        void create_command_buffers(uint32_t graphics_queue);

        const vk::CommandBuffer& current_cmd_buffer() const;

        vk::SwapchainCreateInfoKHR
        swapchain_create_info(const std::vector<uint32_t>&) const;

        vk::SwapchainCreateInfoKHR
        swapchain_create_info(const std::vector<uint32_t>&,
                              const vk::SwapchainKHR&) const;

      public:
        swapchain(std::shared_ptr<device>,
                  device_create_info,
                  std::shared_ptr<const surface>);
        ~swapchain();

        vk::SurfaceTransformFlagBitsKHR current_transform() const;
        const vk::CommandBuffer&        begin_frame();
        vk::Format                      color_format() const;
        vk::Format                      depth_format() const;
        uint32_t                        swapimage_count() const;
        const vk::RenderPass&           get_renderpass() const;
        float                           get_aspect() const;

        void begin_renderpass();
        void end_renderpass();
        void end_frame();

        void recreate_swapchain();

        // no copies
        swapchain(const swapchain&) = delete;
        swapchain& operator=(const swapchain&) = delete;

        // allow move
        swapchain(swapchain&&) = default;
        swapchain& operator=(swapchain&&) = default;
    };

}  // namespace potato::graphics

#endif
