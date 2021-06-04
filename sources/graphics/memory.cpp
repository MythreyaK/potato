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
            physical_device.getMemoryProperties2().memoryProperties
        };

        using mpf = vk::MemoryPropertyFlags;

        auto condition { [&mem_props](mpf _filter, mpf flags, uint32_t i) -> bool {
            return (_filter & mpf(1 << i))
                && ((mem_props.memoryTypes[i].propertyFlags & flags) == flags);
        } };

        for ( uint32_t i = 0; i < mem_props.memoryTypeCount; i++ ) {
            if ( condition(filter, flags, i) ) return i;
        }

        throw std::runtime_error("Failed to find suitable memory type");
    }

    void device::create_buffer(vk::DeviceSize          size,
                               vk::BufferUsageFlags    usage,
                               vk::MemoryPropertyFlags properties,
                               vk::Buffer&             buffer,
                               vk::DeviceMemory&       bufferMemory) const {

        // clang-format off
        vk::BufferCreateInfo buffer_info {
            .size  = size,
            .usage = usage,
            .sharingMode = vk::SharingMode::eExclusive
        };
        // clang-format on

        buffer = logical_device.createBuffer(buffer_info);

        vk::MemoryRequirements mem_req {
            logical_device.getBufferMemoryRequirements(buffer)
        };

        auto flags { vk::MemoryPropertyFlags(mem_req.memoryTypeBits) };

        vk::MemoryAllocateInfo alloc_info {
            .allocationSize  = mem_req.size,
            .memoryTypeIndex = find_mem_type(flags, properties),
        };

        auto dev_mem { logical_device.allocateMemory(alloc_info) };

        logical_device.bindBufferMemory(buffer, dev_mem, 0);
    }

    vk::Image device::create_image(const vk::ImageCreateInfo& im_ci,
                                   vk::MemoryPropertyFlags    props,
                                   vk::DeviceMemory& device_memory) const {

        auto image { logical_device.createImage(im_ci) };

        vk::MemoryRequirements mem_req {
            logical_device.getImageMemoryRequirements(image)
        };

        auto flags { vk::MemoryPropertyFlags(mem_req.memoryTypeBits) };

        vk::MemoryAllocateInfo alloc_info {
            .allocationSize  = mem_req.size,
            .memoryTypeIndex = find_mem_type(flags, props),
        };

        auto img_mem { logical_device.allocateMemory(alloc_info) };

        logical_device.bindImageMemory(image, img_mem, 0);

        return image;
    }

}  // namespace potato::render
