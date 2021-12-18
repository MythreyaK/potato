#include "vma.hpp"

#include "internal.hpp"

namespace vma {
    bool init(vk::PhysicalDevice pd, vk::Device d) {
        internal::phy_device = pd;
        internal::device     = d;

        return true;
    }

    // A way to make this generic?
    template <int N, typename T>
    struct tuple_iterate_call {
        static void free_pools() {
            std::tuple_element_t<N-1, T>::_free_pool();
            tuple_iterate_call<N-1, T>::free_pools();
        }
    };


    template <typename T>
    struct tuple_iterate_call<0, T> {
        static void free_pools() {}
    };

    void deinit() {
        // free all the pools
        internal::device.waitIdle();
        tuple_iterate_call<std::tuple_size_v<all_allocators>, all_allocators>::free_pools();
    }
}  // namespace vma
