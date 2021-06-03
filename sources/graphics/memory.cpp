#include "device.hpp"

namespace potato::render {

    uint32_t device::find_mem_type(vk::MemoryPropertyFlags props,
                                   uint32_t                bit_flags) {

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

        auto cast_to_memflag { [](uint32_t f) {
            return mpf(f);
        } };

        auto condition { [&mem_props](uint32_t bit_flags, uint32_t i) -> bool {
            return (mpf(bit_flags) & mpf(1 << i))
                && ((mem_props.memoryTypes[i].propertyFlags & mpf(bit_flags))
                    == mpf(bit_flags));
        } };

        for ( uint32_t i = 0; i < mem_props.memoryTypeCount; i++ ) {
            if ( condition(bit_flags, i) ) return i;
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

        // vk::MemoryRequirements memory_requirements {
        //     logical_device.getBufferMemoryRequirements(buffer)
        // };

        // vk:MemoryAllocateInfo alloc_info {
        //     .allocationSize = memRequirements.size;
        //     .memoryTypeIndex = find_mem_type(memRequirements.memoryTypeBits,
        //     properties);

        // };

        // if ( vkAllocateMemory(device_, &allocInfo, nullptr, &bufferMemory)
        //      != VK_SUCCESS )
        // {
        //     throw std::runtime_error(
        //       "failed to allocate vertex buffer memory!");
        // }

        // vkBindBufferMemory(device_, buffer, bufferMemory, 0);
    }

}  // namespace potato::render
