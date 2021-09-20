#include "memory.hpp"

#include "utils.hpp"

#include <format>
#include <iostream>
#include <vector>

namespace lol {
    namespace pg = potato::graphics;

    constexpr float MBf { 1024 * 1024 };
    constexpr float GBf { MBf * 1024 };

    // TODO: Lock when allocating

    std::string size_format(vk::DeviceSize sz) {
        std::string a {};
        a.reserve(64);

        auto ret = sz;

        if ( ret > MBf ) {
            a = std::format("{:2.2f} MB ({} B)", ret / MBf, sz);
        }
        if ( ret > GBf ) {
            a = std::format("{:2.2f} GB ({} B)", ret / GBf, sz);
        }
        return a;
    }

    void dump_meminfo(const vk::PhysicalDevice& phydev) {
        auto memprops { phydev.getMemoryProperties2KHR().memoryProperties };

        for ( uint32_t heap_i = 0; heap_i < memprops.memoryHeapCount; ++heap_i )
        {
            // clang-format off
            std::cout << std::format(
                "    Memory heap [{:2}]\n"
                "        Heap size: {}\n"
                "        Heap flags: {}\n",
                heap_i,
                size_format(memprops.memoryHeaps[heap_i].size),
                vk::to_string(memprops.memoryHeaps[heap_i].flags)
            );
            // clang-format on

            for ( uint32_t mem_i = 0; mem_i < memprops.memoryTypeCount;
                  ++mem_i ) {
                if ( memprops.memoryTypes[mem_i].heapIndex == heap_i ) {
                    // clang-format off
                    std::cout << std::format(
                        "          Types [{:2}]: {}\n",
                        mem_i,
                        vk::to_string(memprops.memoryTypes[mem_i].propertyFlags)
                    );
                    // clang-format on
                }
            }
        }
    }

}  // namespace lol

namespace potato::graphics::vma {

    uint32_t allocate_in_pool(pool& this_pool, const vk::MemoryRequirements&);
    uint32_t find_memtype_inx(vk::PhysicalDevice      device,
                              uint32_t                mem_req_bits,
                              vk::MemoryPropertyFlags req_props);

    gpualloc::gpualloc(vk::PhysicalDevice pdev, vk::Device dev)
      : vk_phy_device { pdev }
      , vk_device { dev } {

        auto mem_types {
            pdev.getMemoryProperties2().memoryProperties.memoryTypeCount
        };
        pools.reserve(mem_types);

        for ( auto i = mem_types; i > 0; --i )
            pools.emplace_back(std::vector<pool> {});
    }

    gpualloc::~gpualloc() {
        for ( auto& mem_inx_pool : pools ) {
            for ( auto& pool : mem_inx_pool ) {
                vk_device.free(pool.handle);
            }

            // clear the vector
            mem_inx_pool.clear();
        }
    }

    memory gpualloc::allocate(const vk::MemoryRequirements&  req,
                                 const vk::MemoryPropertyFlags& fl) {
        // find the appropriate memorytype
        auto mem_inx = find_memtype_inx(vk_phy_device, req.memoryTypeBits, fl);

        // get the pool at the mem inx
        auto& mem_inx_pools { pools[mem_inx] };

        if ( mem_inx_pools.size() == 0 ) {
            allocate_pool(mem_inx, mem_inx_pools);
        }

        // allocate from a pool
        for ( uint32_t i = 0; i < mem_inx_pools.size(); ++i ) {
            if ( auto inx = allocate_in_pool(mem_inx_pools[i], req); inx != -1 )
            {
                // clang-format off
                return {
                    .mem_type = mem_inx,
                    .pool_inx = i,
                    .alloc_inx = inx
                };
                // clang-format on
            }
        }

        // came till here, so try again
        allocate_pool(mem_inx, mem_inx_pools);

        auto new_inx = allocate_in_pool(mem_inx_pools.back(), req);
        if ( new_inx == std::numeric_limits<uint32_t>::max() ) {
            throw std::runtime_error("Cannot allocate GPU memory");
        }

        // return mem_inx_pools.back().sub_allocs[inx];
        // clang-format off
        return {
            .mem_type = mem_inx,
            .pool_inx = static_cast<uint32_t>(mem_inx_pools.size() - 1), // last pool
            .alloc_inx = new_inx
        };
        // clang-format on
    }

    void gpualloc::allocate_pool(uint32_t mem_inx, std::vector<pool>& pools) {
        // create pool
        vk::MemoryAllocateInfo info { .allocationSize  = POOL_SIZE,
                                      .memoryTypeIndex = mem_inx };

        pools.emplace_back(pool {
          .handle = vk_device.allocateMemory(info),
        });

        pools.back().sub_allocs.reserve(1000);  // prevent realloc

        // set debug name
        set_debug_name(
          pools.back().handle,
          vk_device,
          { std::format("Pool {} type {}", pools.size() - 1, mem_inx) });
    }

    uint32_t allocate_in_pool(pool&                         this_pool,
                              const vk::MemoryRequirements& req) {

        int i { 0 };

        for ( auto& sub_alloc : this_pool.sub_allocs ) {
            // check for a free suballoc
            if ( sub_alloc.free && req.size <= sub_alloc.size
                 && (sub_alloc.offset % req.alignment) == 0 )
            {
                sub_alloc.free = false;
                sub_alloc.size = req.size;
                return i;
            }
            i++;
        }

        // else allocate a new suballoc. check if can allocate
        // round up to nearest multiple of req.alignment
        if ( POOL_SIZE - this_pool.curr_offset > req.size + req.alignment ) {
            this_pool.curr_offset +=
              req.alignment - (this_pool.curr_offset % req.alignment);

            this_pool.sub_allocs.emplace_back(suballoc {
              .handle = this_pool.handle,
              .size   = req.size,
              .offset = this_pool.curr_offset,
            });

            return this_pool.sub_allocs.size() - 1;
        }

        return std::numeric_limits<uint32_t>::max();
    }

    void gpualloc::deallocate(const memory& s) {
        get(s).free = true;
    }

    suballoc& gpualloc::get(const memory& a) {
        return pools[a.mem_type][a.pool_inx].sub_allocs[a.alloc_inx];
    }

    uint32_t find_memtype_inx(vk::PhysicalDevice      device,
                              uint32_t                mem_req_bits,
                              vk::MemoryPropertyFlags req_props) {
        /*/
        * Quoting from the spec, at
        * https://khronos.org/registry/vulkan/specs/1.2/html/vkspec.html#memory-device-properties
        * https://khronos.org/registry/vulkan/specs/1.2/html/vkspec.html#resources-association
        *
        * Under 'VkMemoryRequirements'
        *
        * "memoryTypeBits is a bitmask and contains one bit set for every
        * supported memory type for the resource. Bit i is set if and only if the
        * memory type i in the VkPhysicalDeviceMemoryProperties structure for the
        * physical device is supported for the resource."
        *
        * So we just check if bit i is set in bit_flags, starting from LSB
        /*/

        auto mem_props { device.getMemoryProperties2().memoryProperties };

        for ( uint32_t i = 0; i < mem_props.memoryTypeCount - 1; ++i ) {
            // clang-format off
            // check if mem properties (such as dev local etc) are satisfied
            bool mem_props_ok {
                (req_props & mem_props.memoryTypes[i].propertyFlags)
                == req_props
            };
            // clang-format on

            // check if bit i is set (memory type i is supported)
            if ( (mem_req_bits & (1 << i)) && mem_props_ok ) {
                return i;
            }
        }

        // I give up
        throw std::runtime_error("Could not find suitable memory type");
    }

}  // namespace potato::graphics
