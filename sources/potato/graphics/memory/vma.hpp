#ifndef POTATO_GRAPHICS_MEMORY_HPP
#define POTATO_GRAPHICS_MEMORY_HPP

#include "allocators/linear.hpp"
#include "memory.hpp"

namespace vma {
    bool init(vk::PhysicalDevice, vk::Device);
}

#endif POTATO_GRAPHICS_MEM_HPP
