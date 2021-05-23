#ifndef POTATO_RENDER_VKINSTANCE_HPP
#define POTATO_RENDER_VKINSTANCE_HPP

#include "vkinclude/vulkan.hpp"

#include <vector>

namespace potato::render {

    vk::UniqueInstance create_instance(std::vector<std::string> extensions);
    std::vector<std::string> get_supported_extensions();
    vk::UniqueDebugUtilsMessengerEXT
    register_debug_callabck(const vk::Instance&);

}  // namespace potato::render

#endif  // !POTATO_VKINSTANCE_HPP
