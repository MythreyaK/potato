#include "device.hpp"

namespace potato::render {

    uint32_t device::find_mem_type(vk::MemoryPropertyFlags filter,
                                   vk::MemoryPropertyFlags flags) const {

        /*/
         * Quoting from the spec, at
         * https://khronos.org/registry/vulkan/specs/1.2-khr-extensions/html/chap12.html#resources-association
         *
         * Under 'VkMemoryRequirements'
         *
         * "memoryTypeBits is a bitmask and contains one bit set for every
         * supported memory type for the resource. Bit i is set if and only if the
         * memory type i in the VkPhysicalDeviceMemoryProperties structure for the
         * physical device is supported for the resource."
         *
         * So we just check if bit i is set in bit_flags, starting from LSB.
         * Higher bits are 'less ideal' for performance reasons.
        /*/

        const auto mem_props {
            physical.getMemoryProperties2().memoryProperties
        };

        // clang-format off
        using mpf = vk::MemoryPropertyFlags;

        auto condition { [&mem_props] // Lambda function to get proper memory index
            (mpf _filter, mpf flags, uint32_t i) -> bool {
                return (_filter & mpf(1 << i))
                    && ((mem_props.memoryTypes[i].propertyFlags & flags) == flags);
        } };

        // clang-format on

        for ( uint32_t i = 0; i < mem_props.memoryTypeCount; i++ ) {
            if ( condition(filter, flags, i) ) return i;
        }

        throw std::runtime_error("Failed to find suitable memory type");
    }

    void device::create_buffer(vk::DeviceSize          size,
                               vk::BufferUsageFlags    usage,
                               vk::MemoryPropertyFlags properties,
                               vk::Buffer&             buffer,
                               vk::DeviceMemory&       buffer_memory) const {

        // clang-format off
        vk::BufferCreateInfo buffer_info {
            .size  = size,
            .usage = usage,
            .sharingMode = vk::SharingMode::eExclusive
        };
        // clang-format on

        buffer = logical->createBuffer(buffer_info);

        vk::MemoryRequirements mem_req {
            logical->getBufferMemoryRequirements(buffer)
        };

        auto flags { vk::MemoryPropertyFlags(mem_req.memoryTypeBits) };

        vk::MemoryAllocateInfo alloc_info {
            .allocationSize  = mem_req.size,
            .memoryTypeIndex = find_mem_type(flags, properties),
        };

        buffer_memory = logical->allocateMemory(alloc_info);

        logical->bindBufferMemory(buffer, buffer_memory, 0);
    }

    vk::Image device::create_image(const vk::ImageCreateInfo& im_ci,
                                   vk::MemoryPropertyFlags    props,
                                   vk::DeviceMemory& device_memory) const {

        auto image { logical->createImage(im_ci) };

        vk::MemoryRequirements mem_req {
            logical->getImageMemoryRequirements(image)
        };

        auto flags { vk::MemoryPropertyFlags(mem_req.memoryTypeBits) };

        vk::MemoryAllocateInfo alloc_info {
            .allocationSize  = mem_req.size,
            .memoryTypeIndex = find_mem_type(flags, props),
        };

        device_memory = logical->allocateMemory(alloc_info);

        logical->bindImageMemory(image, device_memory, 0);

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

}  // namespace potato::render
