#ifndef POTATO_GRAPHICS_UTILS_HPP
#define POTATO_GRAPHICS_UTILS_HPP

#include <type_traits>

namespace potato::graphics {

    template<typename T>
    concept vulkan_handle =
      vk::isVulkanHandleType<std::remove_cvref_t<T>>::value;

    static_assert(vk::isVulkanHandleType<vk::DeviceMemory>::value);
    static_assert(vulkan_handle<vk::DeviceMemory>);

    template<vulkan_handle T>
    constexpr uint64_t to_handle(const T& o) {
        return uint64_t(static_cast<T::CType>(o));
    }

}  // namespace potato::graphics

#endif
