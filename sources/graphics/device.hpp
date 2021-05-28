#ifndef POTATO_RENDER_DEVICE_HPP
#define POTATO_RENDER_DEVICE_HPP

#include "vkinclude/vulkan.hpp"

#include <map>
#include <optional>

extern "C" {
    struct GLFWwindow;
}

namespace potato::render::detail {
    using optional_uint32_t = std::optional<uint32_t>;

    struct _queue {
        optional_uint32_t graphics_inx {};
        optional_uint32_t present_inx {};

        bool is_suitable() {
            return graphics_inx.has_value() && present_inx.has_value();
        }
    };

    struct _swapchain {
        vk::Format         surface_format;
        vk::ColorSpaceKHR  color_space;
        vk::PresentModeKHR present_mode;
        uint32_t           image_count;
    };

    struct device_info {
        _queue                 queues;
        std::string            name;
        vk::PhysicalDevice     device;
        vk::PhysicalDeviceType device_type;
        _swapchain             swapchain;
    };
}  // namespace potato::render::detail

namespace potato::render {
    using vkqueues         = std::map<vk::QueueFlagBits, vk::Queue>;
    using vkswapimages     = std::vector<vk::Image>;
    using vkswapimageviews = std::vector<vk::ImageView>;
    using vkframebuffers   = std::vector<vk::Framebuffer>;

    class device {

      private:
        GLFWwindow*         window_handle {};
        const vk::Instance& instance {};
        vk::SurfaceKHR      surface {};
        detail::device_info device_info {};
        vk::PhysicalDevice  physical_device {};
        vk::UniqueDevice    logical_device {};
        vkqueues            queues {};
        vk::SwapchainKHR    swapchain {};
        vkswapimages        swapimages {};
        vkswapimageviews    swapimageviews {};
        vkframebuffers      framebuffers {};

        void create_swapchain();
        void recreate_swapchain();
        void create_swapchain_stuff();
        void destroy_swapchain_stuff();
        void destroy_framebuffers();

        vk::ShaderModule create_shader(const std::string& filepath) const;

        vk::SwapchainCreateInfoKHR
        swapchain_create_info(const std::vector<uint32_t>&) const;

        vk::SwapchainCreateInfoKHR
        swapchain_create_info(const std::vector<uint32_t>&,
                              const vk::SwapchainKHR&) const;
        // vk::trans current_transform() const

      public:
        device(const vk::Instance& instance, GLFWwindow* window_handle);
        ~device();

        // no copies
        device(const device&) = delete;
        device& operator=(const device&) = delete;

        void create_framebuffers(const vk::RenderPass&);

        vk::SurfaceTransformFlagBitsKHR current_transform() const;
        const vk::Device&               get() const;
        vk::Extent2D                    current_extent() const;
        vk::Format                      format() const;
    };

}  // namespace potato::render

#endif  // !POTATO_RENDER_DEVICE_HPP
