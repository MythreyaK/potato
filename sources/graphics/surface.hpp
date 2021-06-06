#ifndef POTATO_RENDER_SURFACE_HPP
#define POTATO_RENDER_SURFACE_HPP

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

      private:
        GLFWwindow*                   window_handle {};
        std::shared_ptr<const device> potato_device;
        vk::SwapchainKHR              swapchain {};
        vkimages                      swapimages {};
        vkimageviews                  swapimageviews {};
        vkdepthmemory                 depthimagesmemory {};
        vkimages                      depthimages {};
        vkimageviews                  depthimageviews {};
        vkframebuffers                framebuffers {};

        void create_swapchain();
        void create_depth_resources();
        void create_swapchain_stuff();

        void destroy_swapchain_stuff();
        void destroy_framebuffers();

        vk::ShaderModule create_shader(const std::string& filepath) const;

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
        vk::Format                      format() const;
        uint32_t                        swapimage_count() const;
        vk::Format                      find_depth_format() const;

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
