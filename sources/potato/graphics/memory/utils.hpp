#ifndef POTATO_GRAPHICS_MEMORY_UTILS_HPP
#define POTATO_GRAPHICS_MEMORY_UTILS_HPP

#include <cstdint>

namespace vma {

    size_t align(size_t size, size_t align);

    uint32_t find_mem_type(vk::MemoryPropertyFlags filter,
                           uint32_t                mem_type_bits);

}

#endif POTATO_GRAPHICS_MEMORY_UTILS_HPP
