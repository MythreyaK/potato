#include "instance.hpp"

#include "utils.hpp"
#include "version.hpp"

#include <format>
#include <iostream>

extern "C" {
    VKAPI_ATTR VkBool32 VKAPI_CALL
    debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT      messageSeverity,
                   VkDebugUtilsMessageTypeFlagsEXT             messageTypes,
                   const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                   void*                                       pUserData);
}

namespace {
    const std::vector<std::string> required_layers {
#ifdef POTATO_DEBUG
        "VK_LAYER_KHRONOS_validation",
        "VK_LAYER_LUNARG_monitor",
#endif
    };

    const std::vector<std::string> _required_extensions {
        "VK_KHR_get_surface_capabilities2"
    };
}  // namespace

namespace potato::render {

    using str_vec = std::vector<std::string>;

    str_vec get_supported_extensions();
    str_vec get_supported_layers();
    void    add_required_extensions(str_vec&);

    vk::UniqueInstance create_instance(str_vec required_extensions) {

        // Initialize the dynamic loader
        VULKAN_HPP_DEFAULT_DISPATCHER.init(
          dynamicLoader.getProcAddress<PFN_vkGetInstanceProcAddr>(
            "vkGetInstanceProcAddr"));

        const vk::ApplicationInfo app_info {
            .pApplicationName   = "Potato",
            .applicationVersion = 1,
            .pEngineName        = "Potato",
            .engineVersion      = 1,
            .apiVersion         = VK_API_VERSION_1_2,
        };

        const auto supported_extensions { get_supported_extensions() };
        const auto supported_layers { get_supported_layers() };

        if constexpr ( potato::build::is_debug() ) {
            required_extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }

        // adds other extensions we need
        add_required_extensions(required_extensions);
        if ( !has_required_items("Instance extensions", supported_extensions, required_extensions) ) {
            throw std::runtime_error("Required extensions not present");
        }

        if ( !has_required_items("Instance layers", supported_layers, required_layers) ) {
            throw std::runtime_error("Required layers not present");
        }

        // vector-of-string to vector-of-char-* conversion
        std::vector<const char*> extensions {};
        for ( const auto& i : required_extensions ) {
            extensions.push_back(i.c_str());
        }

        std::vector<const char*> layers {};
        for ( const auto& i : required_layers ) {
            layers.push_back(i.c_str());
        }

        const vk::InstanceCreateInfo instance_create_info {
            .pApplicationInfo        = &app_info,
            .enabledLayerCount       = static_cast<uint32_t>(layers.size()),
            .ppEnabledLayerNames     = layers.data(),
            .enabledExtensionCount   = static_cast<uint32_t>(extensions.size()),
            .ppEnabledExtensionNames = extensions.data(),
        };

        auto instance { vk::createInstanceUnique(instance_create_info) };

        VULKAN_HPP_DEFAULT_DISPATCHER.init(instance.get());

        return instance;
    }

    void add_required_extensions(str_vec& req_ext) {
        req_ext.insert(req_ext.end(),
                       _required_extensions.begin(),
                       _required_extensions.end());
    }

    str_vec get_supported_extensions() {

        const auto supported_extns { vk::enumerateInstanceExtensionProperties(
          nullptr) };

        str_vec extns;
        extns.reserve(supported_extns.size());

        for ( const auto& ext : supported_extns ) {
            extns.push_back(ext.extensionName);
        }

        return extns;
    }

    str_vec get_supported_layers() {
        const auto supported_layers { vk::enumerateInstanceLayerProperties() };

        str_vec extns;
        extns.reserve(supported_layers.size());

        for ( const auto& ext : supported_layers ) {
            extns.push_back(ext.layerName);
        }

        return extns;
    }

    vk::UniqueDebugUtilsMessengerEXT register_debug_callabck(
      const vk::Instance& inst) {

        using mtype = vk::DebugUtilsMessageTypeFlagBitsEXT;
        using msevr = vk::DebugUtilsMessageSeverityFlagBitsEXT;

        const vk::DebugUtilsMessengerCreateInfoEXT create_info {
            .flags           = {},
            .messageSeverity = msevr::eError | msevr::eWarning | msevr::eInfo,
            .messageType =
              mtype::eGeneral | mtype::ePerformance | mtype::eValidation,
            .pfnUserCallback = &debug_callback,
            .pUserData       = {}
        };

        auto debug_callback { inst.createDebugUtilsMessengerEXTUnique(
          create_info) };

        return debug_callback;
    }

}  // namespace potato::render

namespace {
    void debug_log(VkDebugUtilsMessageSeverityFlagBitsEXT msev,
                   VkDebugUtilsMessageTypeFlagsEXT        mtypes,
                   VkDebugUtilsMessengerCallbackDataEXT   data,
                   void*                                  puserdata) {

        using vkdmfb = vk::DebugUtilsMessageSeverityFlagBitsEXT;
        using vkdmtf = vk::DebugUtilsMessageTypeFlagsEXT;

        const auto mes_sev { static_cast<vkdmfb>(msev) };
        const auto mes_typs { static_cast<vkdmtf>(mtypes) };

        std::cout << std::format("[{}] - {}]",
                                 vk::to_string(mes_sev),
                                 data.pMessage);
    }
}  // namespace

extern "C" {

    VKAPI_ATTR VkBool32 VKAPI_CALL
    debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT      messageSeverity,
                   VkDebugUtilsMessageTypeFlagsEXT             messageTypes,
                   const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                   void*                                       pUserData) {
        debug_log(messageSeverity, messageTypes, *pCallbackData, pUserData);
        return VK_FALSE;
    }
}
