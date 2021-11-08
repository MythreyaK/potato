#ifndef POTATO_GRAPHICS_MEMORY_ALLOCATOR_LINEAR_HPP
#define POTATO_GRAPHICS_MEMORY_ALLOCATOR_LINEAR_HPP

#include "../internal.hpp"
#include "linear.hpp"

#include <list>
#include <vector>

namespace vma {
    class linear_allocator {
      public:
        struct suballoc;

      private:
        struct pool_metadata {
            // TOOD: List is not efficient for inserts / deletes
            internal::pool        pool {};
            vk::DeviceSize        offset { 0 };
            std::vector<suballoc> suballocs {};
        };

        using pools_t = internal::pool_t<std::vector, pool_metadata>;

        static pools_t pools;

        [[nodiscard]] suballoc* allocate_in_pool(pool_metadata&,
                                                 const vk::MemoryRequirements&,
                                                 const vk::MemoryPropertyFlags&);
        [[nodiscard]] suballoc*
        _allocate(const vk::MemoryRequirements&  mem_req,
                  const vk::MemoryPropertyFlags& mem_flags,
                  int                            depth = 0);

      public:
        struct suballoc {
            vk::DeviceMemory memory {};
            vk::DeviceSize   offset {};
            vk::DeviceSize   size {};
        };

        using suballoc_t = suballoc;

        linear_allocator() = default;

        [[nodiscard]] suballoc_t* allocate(const vk::MemoryRequirements&,
                                           const vk::MemoryPropertyFlags&);
        void                      free();
    };
}  // namespace vma

#endif
