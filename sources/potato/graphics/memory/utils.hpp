#ifndef POTATO_GRAPHICS_MEMORY_UTILS_HPP
#define POTATO_GRAPHICS_MEMORY_UTILS_HPP

#include <cstdint>

namespace vma {

    size_t align(size_t size, size_t align);

    // true if rounded-up start_offset + request_size <= free_size
    bool fits(size_t start_offset,
              size_t request_size,
              size_t free_size,
              size_t alignment);

    uint32_t find_mem_type(vk::MemoryPropertyFlags filter,
                           uint32_t                mem_type_bits);

}  // namespace vma

#endif POTATO_GRAPHICS_MEMORY_UTILS_HPP
