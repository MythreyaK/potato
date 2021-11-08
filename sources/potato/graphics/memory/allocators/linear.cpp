#include "linear.hpp"

#include "../utils.hpp"
#include "core/units.hpp"

#include <utility>

namespace vma {

    linear_allocator::pools_t linear_allocator::pools {};
    // linear_allocator::pools_t linear_allocator::pools {
    //     { { linear_allocator::pool_metadata { .pool = internal::pool(10, 0) }
    //     } }
    // };

    linear_allocator::suballoc*
    linear_allocator::_allocate(const vk::MemoryRequirements&  mem_req,
                                const vk::MemoryPropertyFlags& mem_flags,
                                int                            depth) {
        if ( depth == 2 ) throw std::runtime_error("Allocation failed\n");

        // get the memory type suited
        auto mem_inx { find_mem_type(mem_flags, mem_req.memoryTypeBits) };

        // Allocate from the pool
        for ( auto& pool : pools[mem_inx] ) {
            // check if can allocate in pool
            if ( auto res { allocate_in_pool(pool, mem_req, mem_flags) }; res )
                return res;
        }

        // could not allocate in pools. Create new pool
        if ( pools[mem_inx].size() == 0 ) {
            using namespace units::literals;

            pools[mem_inx].reserve(1024);
            pools[mem_inx].emplace_back(
              pool_metadata { .pool = internal::pool(16_mb, mem_inx) });
        }
        else {
            // allocate twice the size
            internal::pool(pools[mem_inx].back().pool.size * 2, mem_inx);
        }

        // recurse
        return _allocate(mem_req, mem_flags, depth + 1);
    }

    linear_allocator::suballoc_t*
    linear_allocator::allocate(const vk::MemoryRequirements&  mem_req,
                               const vk::MemoryPropertyFlags& mem_flags) {
        return _allocate(mem_req, mem_flags);
    }

    linear_allocator::suballoc* linear_allocator::allocate_in_pool(
      pool_metadata&                 pool_md,
      const vk::MemoryRequirements&  mem_req,
      const vk::MemoryPropertyFlags& mem_flags) {

        // TODO: Try to divide up freed allocations

        // Round up the offset to the alignment
        auto this_offset = align(pool_md.offset, mem_req.alignment);

        pool_md.suballocs.emplace_back(suballoc {
          .memory = pool_md.pool.memory,
          .size   = mem_req.size,
          .offset = this_offset,
        });

        // Increment offset of the pool
        pool_md.offset += mem_req.size;
        pool_md.pool.size -= mem_req.size;

        return &pool_md.suballocs.back();
    }

}  // namespace vma
