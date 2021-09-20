#ifndef POTATO_GRAPHICS_UTILS_HPP
#define POTATO_GRAPHICS_UTILS_HPP

#include <cstdint>
#include <string>
#include <vector>

// namespace debug {
//     std::vector<std::string> debug_obj_names;
// }

namespace potato::graphics::debug {
    extern std::vector<std::string> debug_obj_names;
}

namespace VULKAN_HPP_NAMESPACE {

    template<typename T>
    concept is_handle = requires(T a) {
        T::objectType;
        T::CType;
        std::same_as<vk::isVulkanHandleType<T>, std::true_type>;
    };

    template<is_handle T>
    constexpr uint64_t to_handle(T a) {
        // only way I guess ¯\_(ツ)_/¯
        return reinterpret_cast<uint64_t>(static_cast<T::CType>(a));
    };

}  // namespace VULKAN_HPP_NAMESPACE

namespace potato::graphics {
    template<vk::is_handle T>
    void set_debug_name(const T&          obj,
                        const vk::Device& dev,
                        std::string&&     obj_name) {

        debug::debug_obj_names.emplace_back(std::move(obj_name));

        dev.setDebugUtilsObjectNameEXT({
          .objectType   = T::objectType,
          .objectHandle = vk::to_handle(obj),
          .pObjectName  = debug::debug_obj_names.back().data()
        });
    }
}  // namespace potato::graphics

#endif
