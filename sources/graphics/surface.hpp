#ifndef POTATO_RENDER_SURFACE_HPP
#define POTATO_RENDER_SURFACE_HPP

#include "vkinclude/vulkan.hpp"

extern "C" {
    struct GLFWwindow;
}

namespace potato::render {
    class device;

    class surface {
        using vkswapimages     = std::vector<vk::Image>;
        using vkswapimageviews = std::vector<vk::ImageView>;
        using vkframebuffers   = std::vector<vk::Framebuffer>;
        using vkcmdbuffers     = std::vector<vk::CommandBuffer>;

      private:
        GLFWwindow*           window_handle {};
        const device&         potato_device;
        const vk::SurfaceKHR& render_surface;
        vk::SwapchainKHR      swapchain {};
        vkswapimages          swapimages {};
        vkswapimageviews      swapimageviews {};
        vkframebuffers        framebuffers {};
        vkcmdbuffers          cmd_buffers {};

        void create_swapchain();
        void recreate_swapchain();
        void create_swapchain_stuff();
        void destroy_swapchain_stuff();
        void create_framebuffers(const vk::RenderPass&);
        void destroy_framebuffers();

        vk::ShaderModule create_shader(const std::string& filepath) const;

        vk::SwapchainCreateInfoKHR
        swapchain_create_info(const std::vector<uint32_t>&) const;

        vk::SwapchainCreateInfoKHR
        swapchain_create_info(const std::vector<uint32_t>&,
                              const vk::SwapchainKHR&) const;

        vk::SurfaceTransformFlagBitsKHR current_transform() const;
        vk::Extent2D                    current_extent() const;
        vk::Format                      format() const;

      public:
        surface(GLFWwindow*, const device&);
        ~surface();
    };

}  // namespace potato::render

#endif
