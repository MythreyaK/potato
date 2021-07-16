#include "vulkan.hpp"

#include <format>

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

namespace VULKAN_HPP_NAMESPACE {
    std::string to_string(const vk::ConformanceVersion& c) {
        return std::format("{}.{}.{}", c.major, c.minor, c.patch);
    }
}  // namespace VULKAN_HPP_NAMESPACE
