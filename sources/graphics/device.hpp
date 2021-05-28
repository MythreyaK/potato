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

}  // namespace potato::render::detail

namespace potato::render {
    using vkqueues = std::map<vk::QueueFlagBits, vk::Queue>;

    struct device_settings {
        detail::_queue         queues;
        std::string            name;
        vk::PhysicalDevice     device;
        vk::PhysicalDeviceType device_type;
        detail::_swapchain     swapchain;
    };

    class device {

      private:
        GLFWwindow*         window_handle {};
        const vk::Instance& instance {};
        vk::SurfaceKHR      surface {};
        device_settings     device_info {};
        vk::PhysicalDevice  physical_device {};
        vk::UniqueDevice    logical_device {};
        vkqueues            queues {};

      public:
        device(const vk::Instance& instance, GLFWwindow* window_handle);
        ~device();

        // no copies
        device(const device&) = delete;
        device& operator=(const device&) = delete;

        const vk::Device&         logical() const;
        const vk::PhysicalDevice& physical() const;
        const device_settings&    info() const;
        const vk::SurfaceKHR&     get_surface() const;
        vk::ShaderModule create_shader(const std::string& file_path) const;
    };

}  // namespace potato::render

#endif  // !POTATO_RENDER_DEVICE_HPP
