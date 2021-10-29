#ifndef POTATO_GRAPHICS_MEMORY_LINEAR_ALLOC_HPP
#define POTATO_GRAPHICS_MEMORY_LINEAR_ALLOC_HPP

#include "core/arrayvec.hpp"
#include "internal.hpp"
#include "memory.hpp"

#include <vector>

namespace potato::graphics::vma {

    template<typename T, size_t N>
    using arrayvec = potato::core::arrayvec<T, N>;

    class linear_allocator {
      private:
        struct suballoc_info {
            uint32_t size {};
            uint32_t offset {};
            uint8_t  pool_inx {};
            uint8_t  sub_alloc_inx {};
            uint8_t  mem_type : 5 {};
            bool     free { true };
        };

        struct pool_metadata {
            poolid_t pool_id {};
            uint32_t capacity {};
            uint32_t size {};
            uint32_t curr_offset {};

            std::vector<suballoc_info> sub_allocs {};

            operator bool() {
                return size == 0;
            }
        };

        // pools using this allocator, one per mem type
        using pools_t =
          std::array<std::vector<pool_metadata>, VK_MAX_MEMORY_TYPES>;

        inline static pools_t pools {};

        suballoc_info allocate_in_pool(uint8_t, uint8_t, const vk::MemoryRequirements&);
        const pool&   get_pool(const pool_metadata&);
        suballoc_info _allocate(const vk::MemoryRequirements&,
                                const vk::MemoryPropertyFlags&,
                                int);

        void update_offset(const vk::MemoryRequirements&);

      public:
        suballoc_info  allocate(const vk::MemoryRequirements&,
                                const vk::MemoryPropertyFlags&);
        void           free(const suballoc_info&);
        suballoc_info& get(const suballoc_info&);
    };

    // template<>
    // class memory<linear_allocator> {
    //   private:
    //     // metadata for tracking allocations
    //     uint32_t pool_id {};
    //     uint32_t suballoc_inx {};
    // };

    // using te = memory<linear_allocator>;

}  // namespace potato::graphics::vma

#endif
