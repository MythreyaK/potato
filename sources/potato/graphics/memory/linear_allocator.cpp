#include "linear_allocator.hpp"

namespace potato::graphics::vma {

    linear_allocator::suballoc_info
    linear_allocator::_allocate(const vk::MemoryRequirements&  mem_req,
                                const vk::MemoryPropertyFlags& mem_flags,
                                int                            depth) {

        if ( depth == 2 )
            throw std::runtime_error("Cannot allocate GPU memory");

        // find the appropriate memorytype
        auto mem_inx = find_memtype_inx(mem_req.memoryTypeBits, mem_flags);

        if ( pools[mem_inx][0] ) {
            pools[mem_inx][0] = pool_metadata {
                .pool_id  = pool::allocate(mem_inx, 64 * MB),
                .capacity = 64 * MB,
            };
        }

        // allocate from a pool
        for ( uint32_t pool_inx = 0; pool_inx < pools.size(); ++pool_inx ) {

            if ( auto inx = allocate_in_pool(pools[mem_inx][pool_inx], mem_req);
                 inx != -1 ) {
                // clang-format off

                // clang-format on
            }
        }

        // Couldn't allocate in existing pool, so create a new pool (double the
        // size) and try again
        // pool::allocate(mem_inx, );
        // TODO: Double the size for next allocation

        return _allocate(mem_req, mem_flags, depth + 1);
    }

    linear_allocator::suballoc_info
    linear_allocator::allocate(const vk::MemoryRequirements&  mem_req,
                               const vk::MemoryPropertyFlags& mem_flags) {
        return _allocate(mem_req, mem_flags, 0);
    }

    linear_allocator::suballoc_info
    linear_allocator::allocate_in_pool(uint8_t                       pool_inx,
                                       uint8_t                       mem_inx,
                                       const vk::MemoryRequirements& req) {
        using memtype_t = decltype(suballoc_info::mem_type);

        // TODO: Lock this_pool
        auto& this_pool { pools[mem_inx][pool_inx] };

        for ( auto& sub_alloc : this_pool.sub_allocs ) {
            // check for a free suballoc
            if ( sub_alloc.free && req.size <= sub_alloc.size
                 && (sub_alloc.offset % req.alignment) == 0 )
            {
                // allocate, insert empty alloc
            }
        }

        // else allocate a new suballoc. check if can allocate
        // round up to nearest multiple of req.alignment
        if ( this_pool.sub_allocs.size() < 256
             && this_pool.size - this_pool.curr_offset
                  > req.size + req.alignment )
        {
            // can allocate, return inx of allocation
            // this_pool.curr_offset +=
            //   req.alignment - (this_pool.curr_offset % req.alignment);
            return suballoc_info {
                .size          = static_cast<uint32_t>(req.size),
                .offset        = pools[mem_inx][pool_inx].curr_offset,
                .pool_inx      = static_cast<uint8_t>(pool_inx),
                .sub_alloc_inx = static_cast<uint8_t>(inx),
                .mem_type      = static_cast<memtype_t>(mem_inx),
            };
        }

        // return std::numeric_limits<uint32_t>::max();
    }

    // void linear_allocator::update_offset(const vk::MemoryRequirements& req) {
    //     this_pool.curr_offset +=
    //       req.alignment - (this_pool.curr_offset % req.alignment);
    // }

    const pool& linear_allocator::get_pool(const pool_metadata& p) {
        return pool::get(p.pool_id);
    }

    void linear_allocator::free(const suballoc_info& sa) {
        // get(sa).free = true;
    }

    allocation& linear_allocator::get(const suballoc_info& sa) {
        return pools[sa.pool_inx][sa.pool_inx].sub_allocs[sa.sub_alloc_inx];
    }

}  // namespace potato::graphics::vma
