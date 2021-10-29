#include "internal.hpp"

#include "memory.hpp"

namespace potato::graphics::vma {

    poolid_t pool::allocate(uint32_t mem_type_inx, vk::DeviceSize size) {
        using namespace potato::graphics::vma::internal;

        // TODO: Add lock
        auto mem_handle { g_device.allocateMemory(vk::MemoryAllocateInfo {
          .allocationSize  = size,
          .memoryTypeIndex = mem_type_inx,
        }) };

        auto mem_type_pool_size { g_pools[mem_type_inx].size() };
        auto new_pool_id { poolid_t(mem_type_inx, mem_type_pool_size) };

        g_pools[mem_type_inx][mem_type_pool_size] =
          pool { .id = new_pool_id, .handle = mem_handle, .size = size };

        return new_pool_id;
    }

    poolid_t::poolid_t(uint32_t mem_t, uint32_t pool_inx)
      : mem_type { static_cast<decltype(poolid_t::mem_type)>(mem_t) }
      , g_pool_inx { static_cast<decltype(poolid_t::g_pool_inx)>(pool_inx) } {}

    const pool& pool::get(const poolid_t& pid) {
        return g_pools[pid.mem_type][pid.g_pool_inx];
    }

}  // namespace potato::graphics::vma
