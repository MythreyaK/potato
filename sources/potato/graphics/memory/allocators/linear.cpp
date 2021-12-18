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
            pools[mem_inx].emplace_back(
              internal::pool(pools[mem_inx].back().pool.capacity * 2, mem_inx));
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
        linear_allocator::suballoc_t* ret = nullptr;

        // If the pool is empty, add a new suballoc
        if ( pool_md.suballocs.size() == 0 ) {
            pool_md.suballocs.emplace_back(suballoc {
              .pool   = &pool_md,
              .offset = 0,
              .size   = pool_md.pool.size,
              .free   = true,
            });
        }

        for ( auto sub = pool_md.suballocs.begin();
              sub != pool_md.suballocs.end();
              ++sub )
        {
            if ( sub->free
                 && fits(sub->offset,
                         mem_req.size,
                         sub->size,
                         mem_req.alignment) )
            {
                // suballoc is free, split the boi, by inserting before this
                // alloc, and then reducing the free part's size

                // Round up the offset to the alignment
                auto align_offset = align(sub->offset, mem_req.alignment);

                // clang-format off
                ret = &(*pool_md.suballocs.insert(sub, suballoc {
                    .pool   = &pool_md,
                    .offset = align_offset,
                    .size   = mem_req.size,
                    .free   = false,
                    }));
                // clang-format on

                // now shrink the free region also account for the wasted space
                // due to round up
                auto round_up = sub->offset - ret->offset;
                sub->offset += round_up + ret->size;
                sub->size -= round_up + ret->size;
            }
        }

        // hehe
        return &(*ret);
    }

    void linear_allocator::free(suballoc_t* sub) {
        // to remove it, just mark it free. Check blocks around and merge if needed
        auto& suballocs { sub->pool->suballocs };
        auto  n = sub->offset;
        auto  suballoc =
          std::find_if(suballocs.begin(),
                       suballocs.end(),
                       [n](const auto& val) { return val.offset == n; });

        suballoc->free = true;

        auto merge_suballocs = [&suballocs](auto& suballoc,
                                            auto& next_suballoc) -> void {
            suballoc->size =
              next_suballoc->offset - suballoc->offset + next_suballoc->size;
            suballocs.erase(next_suballoc);
        };

        // if next alloc is free merge them
        if ( suballoc != suballocs.end() ) {
            if ( auto next_sub { std::next(suballoc, +1) }; next_sub->free ) {
                merge_suballocs(suballoc, next_sub);
            }
        }

        // if prev alloc is free merge them
        if ( suballoc != suballocs.begin() ) {
            if ( auto prev_sub { std::next(suballoc, -1) }; prev_sub->free ) {
                merge_suballocs(prev_sub, suballoc);
            }
        }
    }

    bool linear_allocator::suballoc::operator==(const suballoc& other) const {
        return other.offset == offset;
    }

    vk::DeviceMemory linear_allocator::suballoc::memory() const {
        return pool->pool.memory;
    }

    void linear_allocator::_free_pool() {
        for ( auto& mem_inx_pools : pools ) {
            for ( auto& pool : mem_inx_pools ) {
                internal::device.free(pool.pool.memory);
            }
            mem_inx_pools.clear();
        }
    }

}  // namespace vma
