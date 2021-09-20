#ifndef POTATO_GRAPHICS_MEMORY_HPP
#define POTATO_GRAPHICS_MEMORY_HPP

#include <cstddef>

namespace potato::graphics::vma {

    constexpr size_t MB { 1024 * 1024 };
    constexpr size_t GB { MB * 1024 };

    static constexpr auto POOL_SIZE { 128 * MB };

    class gpualloc;

    struct memory {
        uint32_t mem_type;
        uint32_t pool_inx;
        uint32_t alloc_inx;
    };

    struct suballoc {
        vk::DeviceMemory handle {};
        vk::DeviceSize   size {};
        vk::DeviceSize   offset {};
        bool             free { false };
        void*            cpu_ptr { nullptr };
    };

    struct pool {
        vk::DeviceSize        curr_offset { 0 };
        uint32_t              memory_inx {};
        vk::DeviceMemory      handle {};
        std::vector<suballoc> sub_allocs {};

        bool operator==(const pool&) const = default;
    };

    class gpualloc {
      private:
        vk::PhysicalDevice vk_phy_device {};
        vk::Device         vk_device {};
        // each memory type index has many pools
        std::vector<std::vector<pool>> pools {};

        void allocate_pool(uint32_t mem_inx, std::vector<pool>&);

      public:
        gpualloc(vk::PhysicalDevice, vk::Device);
        ~gpualloc();

        memory allocate(const vk::MemoryRequirements&,
                        const vk::MemoryPropertyFlags&);
        void   deallocate(const memory&);

        suballoc& get(const memory&);
    };

}  // namespace potato::graphics

namespace lol {
    void dump_meminfo(const vk::PhysicalDevice& phydev);
}

#endif
