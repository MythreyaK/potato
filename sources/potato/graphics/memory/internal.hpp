#ifndef POTATO_GRAPHICS_VMA_INTERNAL_HPP
#define POTATO_GRAPHICS_VMA_INTERNAL_HPP

#include <cstdint>

namespace potato::graphics::vma {

    struct poolid_t {
        uint16_t mem_type   : 5 { 0 };
        uint16_t g_pool_inx : 10 { 0 };  // index into pool in a mem type
        uint16_t free       : 1 { false };

        poolid_t(uint32_t mem_t, uint32_t pool_inx);
        poolid_t() = default;
    };

    static_assert(sizeof(poolid_t) == 2);

    class pool {
      private:
        /*
        storage for all the pools allocated
        memtype 1 - vector of pools
        memtype 2 - vector of pools
        and so on
        */
        using pools_t = std::array<std::vector<pool>, VK_MAX_MEMORY_TYPES>;

        // All pools across the lifetime of the app
        inline static pools_t g_pools {};

      public:
        poolid_t         id {};
        vk::DeviceMemory handle {};
        vk::DeviceSize   size { 0 };

        static poolid_t    allocate(uint32_t mem_type_inx, vk::DeviceSize size);
        static uint32_t    deallocate(const poolid_t&);
        static const pool& get(const poolid_t&);

        friend void init(vk::PhysicalDevice p, vk::Device d);
    };

}  // namespace potato::graphics::vma

#endif
