#ifndef POTATO_GRAPHICS_MEMORY_INTERNAL_HPP
#define POTATO_GRAPHICS_MEMORY_INTERNAL_HPP

#include <array>

namespace vma::internal {

    extern vk::PhysicalDevice phy_device;
    extern vk::Device         device;

    // Each Memory type has a queue, vector etc of pools
    template<template<typename> class T2, typename T1>
    using pool_t = std::array<T2<T1>, VK_MAX_MEMORY_TYPES>;

    struct pool {
        vk::DeviceMemory memory {};
        vk::DeviceSize   capacity {};
        vk::DeviceSize   size {};

        pool() = default;

        explicit pool(size_t a, uint32_t mem_inx) {
            memory   = internal::device.allocateMemory({
              .allocationSize  = a,
              .memoryTypeIndex = mem_inx,
            });
            capacity = a;
            size = a;
        }
    };
}  // namespace vma::internal

#endif POTATO_GRAPHICS_MEMORY_INTERNAL_HPP
