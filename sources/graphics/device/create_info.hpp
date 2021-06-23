#ifndef POTATO_RENDER_DEVICE_CREATE_INFO_HPP
#define POTATO_RENDER_DEVICE_CREATE_INFO_HPP

// #include "vkinclude/vulkan.hpp"

#include <exception>
#include <optional>

namespace potato::render {
    using opt_inx = std::optional<uint32_t>;

    struct _queue {
        opt_inx graphics;
        opt_inx present;

        bool is_suitable() const {
            return graphics.has_value() && present.has_value();
        }
    };

    struct device_create_info {
        _queue             q_families;
        vk::PhysicalDevice device;
        std::string        device_name;
        vk::Format         surface_format;
        vk::ColorSpaceKHR  color_space;
        vk::PresentModeKHR present_mode;
        uint32_t           image_count;
    };

}  // namespace potato::render

#endif
