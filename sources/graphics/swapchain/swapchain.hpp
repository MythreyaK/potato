#ifndef POTATO_RENDER_SWAPCHAIN_HPP
#define POTATO_RENDER_SWAPCHAIN_HPP

#include "device/create_info.hpp"
#include "pipeline.hpp"
// #include "vkinclude/vulkan.hpp"

extern "C" {
    struct GLFWwindow;
}

namespace potato::render {
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

      public:
        static constexpr int MAX_FRAMES_IN_FLIGHT { 2 };

      private:
        std::shared_ptr<const device>  m_device {};
        device_create_info             m_device_info {};
        std::shared_ptr<const surface> m_surface {};
        vk::SwapchainKHR               m_swapchain {};
        vkimages                       m_swapimages {};
        vkimageviews                   m_swapimageviews {};
        vkdepthmemory                  m_depthimagesmemory {};
        vkimages                       m_depthimages {};
        vkimageviews                   m_depthimageviews {};
        vk::CommandPool                m_cmd_pool {};
        vkcmdbuffers                   m_cmd_buffers {};
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
        void create_swapchain_stuff();

        void destroy_swapchain_stuff();
        void destroy_framebuffers();
        void acquire_image();

        void create_command_buffers(uint32_t graphics_queue);

        const vk::CommandBuffer& current_cmd_buffer() const;

        vk::SwapchainCreateInfoKHR
        swapchain_create_info(const std::vector<uint32_t>&) const;

        vk::SwapchainCreateInfoKHR
        swapchain_create_info(const std::vector<uint32_t>&,
                              const vk::SwapchainKHR&) const;

      public:
        swapchain(std::shared_ptr<const device>,
                  device_create_info,
                  std::shared_ptr<const surface>);
        ~swapchain();

        vk::SurfaceTransformFlagBitsKHR current_transform() const;
        vk::Format                      color_format() const;
        vk::Format                      depth_format() const;
        uint32_t                        swapimage_count() const;

        const vk::CommandBuffer& begin_frame(const vk::RenderPass&);
        void                     end_frame();

        void create_framebuffers(const vk::RenderPass& renderpass);
        void recreate_swapchain();

        // no copies
        swapchain(const swapchain&) = delete;
        swapchain& operator=(const swapchain&) = delete;

        // allow move
        swapchain(swapchain&&) = default;
        swapchain& operator=(swapchain&&) = default;
    };

}  // namespace potato::render

#endif
