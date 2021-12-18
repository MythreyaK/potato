#ifndef POTATO_GRAPHICS_DEBUG_NAME_UTILS_HPP
#define POTATO_GRAPHICS_DEBUG_NAME_UTILS_HPP

#include "utils.hpp"

#include <string>
#include <vector>

namespace potato::graphics::internal {
    // storage for all the debug names
    extern std::vector<std::string> debug_names;
}  // namespace potato::graphics::internal

namespace potato::graphics {

    template<typename T>
    void _set_debug_name(const T& obj, vk::Device& dev, const char* pname) {
        // clang-format off
        vk::DebugUtilsObjectNameInfoEXT name_info {
            .objectType = T::objectType,
            .objectHandle = to_handle(obj),
            .pObjectName = pname,
        };
        // clang-format on

        dev.setDebugUtilsObjectNameEXT(name_info);
    }

    template<typename T>
    void set_debug_name(const T& obj, vk::Device& dev, std::string&& dbg_name) {
        internal::debug_names.emplace_back(std::move(dbg_name));
        _set_debug_name(obj, dev, internal::debug_names.back().c_str());
    }

    template<typename T>
    void set_debug_name(const T& obj, vk::Device& dev, const char* pname) {
        _set_debug_name(obj, dev, pname);
    }

}  // namespace potato::graphics

#endif
