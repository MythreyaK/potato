#ifndef VKINCLUDE_HPP
#define VKINCLUDE_HPP
// This file ensures all vulkan.hpp includes in the
// project use these preprosessor directives

#define NOMINMAX  // windows.h hash-defines max macro, breaking stuff
#define WIN32_LEAN_AND_MEAN
#define VK_USE_PLATFORM_WIN32_KHR
#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS  // enable designated initializers
#define VULKAN_HPP_NO_SPACESHIP_OPERATOR   // using <=> is apparently slow and
                                           // incomplete. See docs

#include <string>
#include <vulkan/vulkan.hpp>

namespace VULKAN_HPP_NAMESPACE {
    std::string to_string(const vk::ConformanceVersion&);
}

static vk::DynamicLoader dynamicLoader;

#endif  // !VKINCLUDE_HPP
