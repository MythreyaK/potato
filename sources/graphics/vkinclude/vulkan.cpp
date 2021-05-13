#include "vulkan.hpp"

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

namespace potato::graphics {
    static vk::DynamicLoader dynamicLoader {};
}
