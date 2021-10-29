#ifndef POTATO_GRAPHICS_MEMORY2_HPP
#define POTATO_GRAPHICS_MEMORY2_HPP

#include <concepts>
#include <cstddef>

namespace potato::graphics::vma {

    void     init(vk::PhysicalDevice, vk::Device);
    uint32_t find_memtype_inx(uint32_t                mem_req_bits,
                              vk::MemoryPropertyFlags req_props);

    constexpr size_t MB { 1024 * 1024 };
    constexpr size_t GB { MB * 1024 };

    struct allocation;

    // clang-format off
    template<typename some_long_t>
    concept gpuallocator = requires(some_long_t                    alloc,
                                    const vk::MemoryRequirements&  a,
                                    const vk::MemoryPropertyFlags& b) {
        some_long_t::suballoc_info; // for mapping suballocs to pools
        // alloc.free(some_long_t::suballoc_metadata_t id);
        // { alloc.allocate(a, b) } -> std::same_as<some_long_t::suballoc_info>;
        // { alloc.get(id) } -> std::same_as<allocation&>;
    };
    // clang-format on

    template<gpuallocator T, int F>
    class memory {
      private:
        using gpuallocator_t  = T;
        using suballoc_info_t = gpuallocator_t::suballoc_info;

        gpuallocator_t          allocator {};
        suballoc_info_t         metadata {};
        // vk::MemoryPropertyFlags usage_flags { F };

      public:
        memory() = default;
        memory(const vk::MemoryRequirements& r, const vk::MemoryPropertyFlags& f)
          : allocator { gpuallocator_t {} }
          , metadata { allocator.allocate(r, f) } {}

        allocation& get() {
            return allocator.get(metadata);
        }

        // void map();
        // void flush_to_gpu();
        // void refresh_from_gpu();
    };

    struct allocation {
        vk::DeviceMemory handle {};
        vk::DeviceSize   size {};
        vk::DeviceSize   offset {};
        bool             free { false };
    };

    void dump_meminfo();

}  // namespace potato::graphics::vma

namespace potato::graphics::vma::internal {
    extern vk::PhysicalDevice g_phy_device;
    extern vk::Device         g_device;
}  // namespace potato::graphics::vma::internal

namespace vma = potato::graphics::vma;

#endif
