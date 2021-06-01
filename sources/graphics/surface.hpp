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

      private:
        GLFWwindow*           window_handle {};
        const device&         potato_device;
        const vk::SurfaceKHR& render_surface;
        vk::SwapchainKHR      swapchain {};
        vkswapimages          swapimages {};
        vkswapimageviews      swapimageviews {};

        void create_swapchain();
        void recreate_swapchain();
        void create_swapchain_stuff();
        void destroy_swapchain_stuff();

        vk::ShaderModule create_shader(const std::string& filepath) const;

        vk::SwapchainCreateInfoKHR
        swapchain_create_info(const std::vector<uint32_t>&) const;

        vk::SwapchainCreateInfoKHR
        swapchain_create_info(const std::vector<uint32_t>&,
                              const vk::SwapchainKHR&) const;

      public:
        surface(GLFWwindow*, const device&);
        ~surface();

        vk::SurfaceTransformFlagBitsKHR current_transform() const;
        vk::Extent2D                    current_extent() const;
        vk::Format                      format() const;

        void reinitialize();
    };

}  // namespace potato::render

#endif
