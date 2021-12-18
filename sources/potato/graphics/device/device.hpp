#ifndef POTATO_RENDER_DEVICE_HPP
#define POTATO_RENDER_DEVICE_HPP

#include "create_info.hpp"
// #include "vkinclude/vulkan.hpp"

#include <map>
#include <optional>

namespace potato::graphics {
    class surface;

    using vkqueues = std::map<vk::QueueFlagBits, vk::Queue>;

    // TODO: Maybe use a tagged shared_ptr
    class device : public std::enable_shared_from_this<device> {

      public:
        const device_create_info create_info {};
        const vk::PhysicalDevice physical {};
        const vk::UniqueDevice   logical {};
        const vkqueues           queues {};

        ~device();
        device(const vk::Instance&, const surface&);

        // no copies
        device(const device&) = delete;
        device& operator=(const device&) = delete;

        // allow move
        device(device&&) = default;
        device& operator=(device&&) = default;

        uint32_t find_mem_type(vk::MemoryPropertyFlags props,
                               vk::MemoryPropertyFlags bit_flags) const;

        vk::Format
        find_supported_format(const std::vector<vk::Format>& candidates,
                              vk::ImageTiling                tiling,
                              vk::FormatFeatureFlags         features) const;
    };

}  // namespace potato::graphics

#endif  // !POTATO_RENDER_DEVICE_HPP
