#ifndef POTATO_RENDER_DEVICE_HPP
#define POTATO_RENDER_DEVICE_HPP

#include "create_info.hpp"
#include "graphics/memory/memory.hpp"
#include "graphics/utils.hpp"

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

      public:
        ~device();
        device(const vk::Instance&, const surface&);

        // no copies
        device(const device&) = delete;
        device& operator=(const device&) = delete;

        // allow move
        device(device&&) = default;
        device& operator=(device&&) = default;

        // vk::Image create_image(vk::ImageCreateInfo,
        //                        vk::MemoryPropertyFlags,
        //                        vma::memory&,
        //                        vk::Extent3D max_extent);

        vk::Format
        find_supported_format(const std::vector<vk::Format>& candidates,
                              vk::ImageTiling                tiling,
                              vk::FormatFeatureFlags         features) const;

        // void create_buffer(vk::DeviceSize,
        //                    vk::BufferUsageFlags,
        //                    vk::MemoryPropertyFlags,
        //                    vk::Buffer&,
        //                    vma::memory&);
    };

}  // namespace potato::graphics

#endif  // !POTATO_RENDER_DEVICE_HPP
