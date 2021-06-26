#include "instance.hpp"

#include "utils/utils.hpp"
#include "version.hpp"

#include <format>

#pragma region CTYPE_AND_CONSTANTS
extern "C" {
    typedef VkDebugUtilsMessageTypeFlagsEXT        cmtf;
    typedef VkDebugUtilsMessageSeverityFlagBitsEXT cmsfb;
    typedef VkDebugUtilsMessengerCallbackDataEXT   cmcd;

    // clang-format off
    VKAPI_ATTR VkBool32 VKAPI_CALL
    debug_callback(cmsfb, cmtf, const cmcd*, void*);
    // clang-format on

    const char** glfwGetRequiredInstanceExtensions(uint32_t*);
    void         debug_log(cmsfb msev, cmtf mtypes, cmcd data, void* puserdata);
}

namespace {
    const std::vector<std::string> _required_layers {
#ifdef POTATO_DEBUG
        "VK_LAYER_KHRONOS_validation",
        "VK_LAYER_LUNARG_monitor",
#endif
    };

    const std::vector<std::string> _required_extensions {
        "VK_KHR_get_surface_capabilities2"
    };
}  // namespace

#pragma endregion CTYPE_AND_CONSTANTS

namespace potato::graphics {
#pragma region FWD_DECLARE_MISC
    using cmtf    = VkDebugUtilsMessageTypeFlagsEXT;
    using cmsfb   = VkDebugUtilsMessageSeverityFlagBitsEXT;
    using cmcd    = VkDebugUtilsMessengerCallbackDataEXT;
    using mtf     = vk::DebugUtilsMessageTypeFlagsEXT;
    using mtfb    = vk::DebugUtilsMessageTypeFlagBitsEXT;
    using msf     = vk::DebugUtilsMessageSeverityFlagsEXT;
    using msfb    = vk::DebugUtilsMessageSeverityFlagBitsEXT;
    using dbci    = vk::DebugUtilsMessengerCreateInfoEXT;
    using icrf    = vk::StructureChain<vk::InstanceCreateInfo, dbci>;
    using strings = std::vector<std::string>;
    using vecchr  = std::vector<const char*>;

    strings supported_layers();
    strings required_extensions();

    constexpr dbci debug_create_info() {

        msf mess_severity { msfb::eError };
        mtf mess_types { mtfb::ePerformance };

        if constexpr ( potato::build::is_debug() ) {
            mess_severity |= msfb::eWarning;
            mess_types |= mtfb::eValidation | mtfb::eGeneral;
        }

        return { .flags           = {},
                 .messageSeverity = mess_severity,
                 .messageType     = mess_types,
                 .pfnUserCallback = &debug_callback,
                 .pUserData       = {} };
    }

#pragma endregion FWD_DECLARE_MISC

    instance::instance(std::vector<std::string> extensions) {
        using namespace potato::utils;

        // Initialize the dynamic loader
        VULKAN_HPP_DEFAULT_DISPATCHER.init(
          dynamicLoader.getProcAddress<PFN_vkGetInstanceProcAddr>(
            "vkGetInstanceProcAddr"));

        const vk::ApplicationInfo app_info {
            .pApplicationName   = "Potato App",
            .applicationVersion = 1,
            .pEngineName        = "Potato",
            .engineVersion      = 1,
            .apiVersion         = VK_API_VERSION_1_2,
        };

        if constexpr ( potato::build::is_debug() ) {
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }

        // Push in required extensions
        auto rqx { required_extensions() };
        extensions.insert(extensions.end(),
                          std::make_move_iterator(rqx.begin()),
                          std::make_move_iterator(rqx.end()));

        if ( !has_required_items("Instance extensions",
                                 supported_extensions(),
                                 extensions) )
        {
            throw std::runtime_error("Required extensions not present");
        }

        if ( !has_required_items("Instance layers",
                                 supported_layers(),
                                 _required_layers) )
        {
            throw std::runtime_error("Required layers not present");
        }

        const auto _extns { to_vecchar(extensions) };
        const auto _layers { to_vecchar(_required_layers) };

        // Also add debug messenger create info
        constexpr auto debug_crinf { debug_create_info() };

        const icrf inst_create_info {
            vk::InstanceCreateInfo {
              .pApplicationInfo        = &app_info,
              .enabledLayerCount       = static_cast<uint32_t>(_layers.size()),
              .ppEnabledLayerNames     = _layers.data(),
              .enabledExtensionCount   = static_cast<uint32_t>(_extns.size()),
              .ppEnabledExtensionNames = _extns.data(),
            },
            debug_crinf
        };

        vkinstance = vk::createInstanceUnique(
          inst_create_info.get<vk::InstanceCreateInfo>());

        VULKAN_HPP_DEFAULT_DISPATCHER.init(vkinstance.get());
    }

    strings instance::supported_extensions() {

        const auto supported_extns { vk::enumerateInstanceExtensionProperties(
          nullptr) };

        strings extns;
        extns.reserve(supported_extns.size());

        for ( const auto& ext : supported_extns ) {
            extns.push_back(ext.extensionName);
        }

        return extns;
    }

    const vk::Instance& instance::get() {
        return vkinstance.get();
    }

    strings required_extensions() {
        strings ret { _required_extensions };

        auto ext_count { 0u };
        auto ext_charptr { glfwGetRequiredInstanceExtensions(&ext_count) };
        ret.insert(ret.end(), ext_charptr, ext_charptr + ext_count);
        return ret;
    }

    strings supported_layers() {
        const auto supported_layers { vk::enumerateInstanceLayerProperties() };

        strings extns;
        extns.reserve(supported_layers.size());

        for ( const auto& ext : supported_layers ) {
            extns.push_back(ext.layerName);
        }

        return extns;
    }

    void debug_log(cmsfb msev, cmtf mtypes, cmcd data, void* puserdata) {

        const auto mes_sev { static_cast<msf>(msev) };
        const auto mes_typs { static_cast<mtf>(mtypes) };

        std::cout << std::format("[{} {}] - [{}] \n",
                                 vk::to_string(mes_sev),
                                 vk::to_string(mes_typs),
                                 data.pMessage);
    }
}  // namespace potato::graphics

extern "C" {
    // clang-format off
    VKAPI_ATTR VkBool32 VKAPI_CALL
    debug_callback(cmsfb       messageSeverity,
                   cmtf        messageTypes,
                   const cmcd* pCallbackData,
                   void*       pUserData)
    {
        potato::graphics::debug_log(messageSeverity, messageTypes, *pCallbackData, pUserData);
        return VK_FALSE;
    }
    // clang-format on
}
