#include "utils.hpp"

#include "internal.hpp"

namespace {
    bool memtype_suitable(vk::MemoryPropertyFlags f, uint32_t bits, int inx) {

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

        using namespace vma::internal;
        using mpf = vk::MemoryPropertyFlags;
        auto _bits { static_cast<mpf>(bits) };

        const auto mem_props {
            phy_device.getMemoryProperties2().memoryProperties
        };
        // clang-format off
        bool bit_i_present { _bits & mpf(1 << inx) };   // that mem type is supported
        bool has_req_flag {                             // that mem flag is supported
            (mem_props.memoryTypes[inx].propertyFlags & f) == f
        };
        // clang-format on

        return bit_i_present & has_req_flag;
    }
}  // namespace

namespace vma {
    uint32_t find_mem_type(vk::MemoryPropertyFlags filter, uint32_t bits) {

        const auto mem_props {
            internal::phy_device.getMemoryProperties2().memoryProperties
        };

        for ( uint32_t i = 0; i < mem_props.memoryTypeCount; i++ ) {
            if ( memtype_suitable(filter, bits, i) ) return i;
        }

        throw std::runtime_error("Failed to find suitable memory type");
    }

    size_t align(size_t s, size_t a) {
        return s % a == 0 ? s : s + (a - s % a);
    }

    bool fits(size_t start_offset,
              size_t request_size,
              size_t free_size,
              size_t alignment) {
        auto align_offset = align(start_offset, alignment);
        return (align_offset - start_offset) + request_size <= free_size;
    }
}  // namespace vma
