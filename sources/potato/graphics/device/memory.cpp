#include "device.hpp"

namespace potato::graphics {

    void device::create_buffer(vk::DeviceSize          size,
                               vk::BufferUsageFlags    usage,
                               vk::MemoryPropertyFlags properties,
                               vk::Buffer&             buffer) {

        // clang-format off
        vk::BufferCreateInfo buffer_info {
            .size  = size,
            .usage = usage,
            .sharingMode = vk::SharingMode::eExclusive
        };
        // clang-format on

        buffer = logical->createBuffer(buffer_info);

        vk::MemoryRequirements mem_req { logical->getBufferMemoryRequirements(
          buffer) };

        auto res = allocator.allocate(mem_req, properties);

        auto& alloc = allocator.get(res);

        logical->bindBufferMemory(buffer, alloc.handle, alloc.offset);
    }

    vk::Image device::create_image(const vk::ImageCreateInfo& im_ci,
                                   vk::MemoryPropertyFlags    props) {

        auto image { logical->createImage(im_ci) };

        vk::MemoryRequirements mem_req { logical->getImageMemoryRequirements(
          image) };

        auto res = allocator.allocate(mem_req, props);

        auto& alloc = allocator.get(res);

        logical->bindImageMemory(image, alloc.handle, alloc.offset);

        return image;
    }

    vk::Format
    device::find_supported_format(const std::vector<vk::Format>& candidates,
                                  vk::ImageTiling                tiling,
                                  vk::FormatFeatureFlags features) const {

        for ( const auto& format : candidates ) {
            vk::FormatProperties props {
                physical.getFormatProperties2(format).formatProperties
            };

            if ( tiling == vk::ImageTiling::eLinear
                 && (props.linearTilingFeatures & features) == features )
            {
                return format;
            }
            else if ( tiling == vk::ImageTiling::eOptimal
                      && (props.optimalTilingFeatures & features) == features )
            {
                return format;
            }
        }

        throw std::runtime_error("Could not find supported format");
    }

}  // namespace potato::graphics
