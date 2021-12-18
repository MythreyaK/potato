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
            internal::pool      pool {};
            std::list<suballoc> suballocs {};
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
            pool_metadata* pool {};
            vk::DeviceSize offset {};
            vk::DeviceSize size {};
            bool           free { false };

            bool             operator==(const suballoc& other) const;
            vk::DeviceMemory memory() const;
        };

        using suballoc_t = suballoc;

        static void _free_pool();

        linear_allocator() = default;

        [[nodiscard]] suballoc_t* allocate(const vk::MemoryRequirements&,
                                           const vk::MemoryPropertyFlags&);
        void                      free(suballoc_t*);
    };
}  // namespace vma

#endif
