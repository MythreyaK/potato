#ifndef POTATO_GRAPHICS_MEMORY_MEM_HPP
#define POTATO_GRAPHICS_MEMORY_MEM_HPP

#include <concepts>
#include <cstdint>

namespace vma {

    class linear_allocator;

    template<typename sub_t>
    concept allocator_suballoc = requires(sub_t a) {
        { a.memory() } -> std::same_as<vk::DeviceMemory>;
    };

    template<typename class_type>
    concept gpu_allocator = requires(class_type                     a,
                                     const vk::MemoryRequirements&  mr,
                                     const vk::MemoryPropertyFlags& mf) {
        // clang-format off
        class_type::suballoc_t;
        // allocator_suballoc<class_type::suballoc_t>;
        // { a.allocate(mr, mf) } -> std::same_as<
        //         std::add_pointer<class_type::suballoc_t>
        //     >;
        // clang-format on
    };

    static_assert(allocator_suballoc<linear_allocator::suballoc_t>);

    template<gpu_allocator T = linear_allocator>
    class memory {
      private:
        using allocator = T;
        using suballoc  = T::suballoc_t;

        allocator m_allocator {};
        suballoc* m_suballoc {};

        void* cpu_map { nullptr };

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

        memory& bind(const vk::Image& image) {
            internal::device.bindImageMemory(image,
                                             m_suballoc->memory(),
                                             m_suballoc->offset);
            return *this;
        }

        memory& bind(const vk::Buffer& buffer) {
            internal::device.bindBufferMemory(buffer,
                                              m_suballoc->memory(),
                                              m_suballoc->offset);
            return *this;
        }

        memory& map() {
            std::ignore = internal::device.mapMemory(m_suballoc->memory(),
                                                     m_suballoc->offset,
                                                     m_suballoc->size,
                                                     {},
                                                     &cpu_map);
            return *this;
        }

        memory& unmap() {
            internal::device.unmapMemory(m_suballoc->memory());
            cpu_map = nullptr;
            return *this;
        }

        memory& write_to_gpu(const void* data, size_t size) {
            assert(cpu_map != nullptr);
            std::memcpy(cpu_map, data, size);
            return *this;
        }

        // void flush_to_gpu();
        // void fetch_from_gpu();
    };

}  // namespace vma

#endif POTATO_GRAPHICS_MEMORY_MEM_HPP
