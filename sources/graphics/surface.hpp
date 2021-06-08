#ifndef POTATO_RENDER_SURFACE_HPP
#define POTATO_RENDER_SURFACE_HPP

#include "pipeline.hpp"
#include "vkinclude/vulkan.hpp"

extern "C" {
    struct GLFWwindow;
}

namespace potato::render {
    class device;

    class surface {
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
        GLFWwindow*                   window_handle {};
        std::shared_ptr<const device> potato_device;
        vk::SwapchainKHR              swapchain {};
        vkimages                      swapimages {};
        vkimageviews                  swapimageviews {};
        vkdepthmemory                 depthimagesmemory {};
        vkimages                      depthimages {};
        vkimageviews                  depthimageviews {};
        vk::CommandPool               cmd_pool {};
        vkcmdbuffers                  cmd_buffers {};
        vkframebuffers                framebuffers {};

        vksemaphores image_available {};  // pipeline waits for this before write
        vksemaphores render_complete {};
        vkfences     in_flight_fence {};
        vkfences     in_flight_image {};
        uint32_t     current_frame { 0 };
        uint32_t     framebuffer_index { 0 };
        bool         frame_in_progress { false };

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
        surface(GLFWwindow*, std::shared_ptr<const device>);
        ~surface();

        vk::SurfaceTransformFlagBitsKHR current_transform() const;
        vk::Extent2D                    current_extent() const;
        vk::Format                      color_format() const;
        vk::Format                      depth_format() const;
        uint32_t                        swapimage_count() const;

        const vk::CommandBuffer& begin_frame(const vk::RenderPass&,
                                             const pipeline&);
        void                     end_frame();

        void create_framebuffers(const vk::RenderPass& renderpass);
        void recreate_swapchain();

        // no copies
        surface(const surface&) = delete;
        surface& operator=(const surface&) = delete;

        // allow move
        surface(surface&&) = default;
        surface& operator=(surface&&) = default;
    };

}  // namespace potato::render

#endif
