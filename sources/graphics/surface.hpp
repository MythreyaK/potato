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
        GLFWwindow*                   window_handle {};
        std::shared_ptr<const device> potato_device;
        vk::SwapchainKHR              swapchain {};
        vkswapimages                  swapimages {};
        vkswapimageviews              swapimageviews {};

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
        surface(GLFWwindow*, std::shared_ptr<const device>);
        ~surface();

        vk::SurfaceTransformFlagBitsKHR current_transform() const;
        vk::Extent2D                    current_extent() const;
        vk::Format                      format() const;
        uint32_t                        swapimage_count() const;

        // no copies
        surface(const surface&) = delete;
        surface& operator=(const surface&) = delete;

        // allow move
        surface(surface&&) = default;
        surface& operator=(surface&&) = default;
    };

}  // namespace potato::render

#endif
