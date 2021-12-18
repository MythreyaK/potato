#ifndef POTATO_GRAPHICS_MEMORY_HPP
#define POTATO_GRAPHICS_MEMORY_HPP

#include "allocators/linear.hpp"
#include "memory.hpp"

#include <tuple>

using all_allocators = std::tuple<vma::linear_allocator>;

namespace vma {
    bool init(vk::PhysicalDevice, vk::Device);
    void deinit();
}  // namespace vma

#endif POTATO_GRAPHICS_MEM_HPP
