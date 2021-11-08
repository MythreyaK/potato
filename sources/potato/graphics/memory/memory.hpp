#ifndef POTATO_GRAPHICS_MEMORY_MEM_HPP
#define POTATO_GRAPHICS_MEMORY_MEM_HPP

#include <concepts>
#include <cstdint>

namespace vma {

    class linear_allocator;

    template<typename class_type>
    concept gpu_allocator = requires(class_type a,
                                     const vk::MemoryRequirements& mr,
                                     const vk::MemoryPropertyFlags& mf) {
        // clang-format off
        class_type::suballoc_t;
        // { a.allocate(mr, mf) } -> std::same_as<
        //         std::add_pointer<class_type::suballoc_t>
        //     >;
        // clang-format on
    };

    template<gpu_allocator T = linear_allocator>
    class memory {
      private:
        using allocator = T;
        using suballoc  = T::suballoc_t;

        allocator m_allocator {};
        suballoc* m_suballoc {};

      public:
        memory() = default;

        memory(const vk::MemoryRequirements& req, vk::MemoryPropertyFlags props)
          : m_allocator { allocator() }
          , m_suballoc { m_allocator.allocate(req, props) } {}

        // no copy
        memory(const memory&) = delete;
        memory operator=(const memory&) = delete;

        // no copy
        memory(memory&&) = default;
        memory& operator=(memory&&) = default;

        void free() {
            m_allocator.free(m_suballoc);
        }

        // void map();
        // void flush_to_gpu();
        // void fetch_from_gpu();
    };

}  // namespace vma

#endif POTATO_GRAPHICS_MEMORY_MEM_HPP
