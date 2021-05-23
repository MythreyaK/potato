#include "device.hpp"

#include "utils.hpp"

#include <optional>
#include <set>
#include <utility>

extern "C" {
    VkResult glfwCreateWindowSurface(VkInstance,
                                     GLFWwindow*,
                                     const VkAllocationCallbacks*,
                                     VkSurfaceKHR*);
}

namespace potato::render {

#pragma region FWD_DECLARE
    using optional_uint32_t = std::optional<uint32_t>;

    struct q_info {
        optional_uint32_t graphics_queue_inx {};
        optional_uint32_t present_queue_inx {};

        bool is_suitable() {
            return graphics_queue_inx.has_value()
                && present_queue_inx.has_value();
        }
    };

    struct swapchain_info {
        vk::Format         surface_format;
        vk::ColorSpaceKHR  color_space;
        vk::PresentModeKHR present_mode;
    };

    struct device_info {
        q_info                 queues;
        vk::PhysicalDevice     device;
        vk::PhysicalDeviceType device_type;
        swapchain_info         swapchain_info;
    };

    const std::vector<std::string> required_device_extensions {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    vk::UniqueDevice create_device(device_info device);
    device_info      pick_device(const vk::Instance&, const vk::SurfaceKHR&);
    vk::SurfaceKHR   create_surface(const vk::Instance&, GLFWwindow*);
    std::optional<device_info> get_device_suitable(const vk::PhysicalDevice&,
                                                   const vk::SurfaceKHR&);

#pragma endregion FWD_DECLARE

    /**** device class ****/

    device::device(const vk::Instance& instance, GLFWwindow* window_handle)
      : instance(instance) {

        surface            = create_surface(instance, window_handle);
        auto picked_device = pick_device(instance, surface);
        physical_device    = picked_device.device;
        logical_device     = create_device(picked_device);
    }

    device::~device() {
        instance.destroySurfaceKHR(surface);
    }

    vk::UniqueDevice create_device(device_info device_info) {

        std::set<uint32_t> queues {
            device_info.queues.graphics_queue_inx.value(),
            device_info.queues.present_queue_inx.value()
        };

        std::vector<vk::DeviceQueueCreateInfo> queue_create_infos {};

        const auto queuePriority = 1.0f;
        for ( auto queue : queues ) {

            queue_create_infos.emplace_back(vk::DeviceQueueCreateInfo {
              .queueFamilyIndex = queue,
              .queueCount       = 1,
              .pQueuePriorities = &queuePriority,
            });
        }

        // extensions
        std::vector<const char*> extns {};
        for ( const auto& i : required_device_extensions ) {
            extns.push_back(i.c_str());
        }

        // TODO: Device features, and optional extensions
        const auto create_info { vk::DeviceCreateInfo {
          .queueCreateInfoCount =
            static_cast<uint32_t>(queue_create_infos.size()),
          .pQueueCreateInfos       = queue_create_infos.data(),
          .enabledExtensionCount   = static_cast<uint32_t>(extns.size()),
          .ppEnabledExtensionNames = extns.data(),
          .pEnabledFeatures        = {},
        } };

        return device_info.device.createDeviceUnique(create_info);
    }

#pragma region UTILS

    device_info pick_device(const vk::Instance&   inst,
                            const vk::SurfaceKHR& srf) {

        auto devices { inst.enumeratePhysicalDevices() };

        std::vector<device_info> suitable_devices {};

        for ( auto& d : devices ) {
            auto info { get_device_suitable(d, srf) };
            if ( info.has_value() ) {
                suitable_devices.emplace_back(info.value());
            }
        }

        if ( suitable_devices.size() == 0 ) {
            throw std::runtime_error("Failed to find suitable gpu");
        }
        else {
            return suitable_devices[0];
        }
    }

    std::optional<device_info> get_device_suitable(
      const vk::PhysicalDevice& device,
      const vk::SurfaceKHR&     surface) {

        using qfb = vk::QueueFlagBits;

        device_info info {};

        const auto features { device.getFeatures2() };
        const auto properties { device.getProperties2() };

        const auto queues { device.getQueueFamilyProperties2() };

        // device
        info.device_type = properties.properties.deviceType;
        info.device      = device;

        // Get the queues
        uint32_t i = 0;
        for ( const auto& queue : queues ) {
            const auto props { queue.queueFamilyProperties };

            if ( props.queueFlags & qfb::eGraphics
                 && !info.queues.graphics_queue_inx.has_value() )
            {
                info.queues.graphics_queue_inx = i;
            }

            if ( device.getSurfaceSupportKHR(i, surface) ) {
                info.queues.present_queue_inx = i;
            }

            if ( info.queues.is_suitable() ) break;

            ++i;
        }

        // get extensions
        const auto device_extensions {
            device.enumerateDeviceExtensionProperties()
        };
        std::vector<std::string> supported_extns {};
        for ( const auto ext : device_extensions ) {
            supported_extns.push_back(ext.extensionName);
        }

        if ( !has_required_items("Device extensions", supported_extns, required_device_extensions) )
        {
            return {};
        }

        // Get the swapchain info
        bool can_present { true };

        vk::PhysicalDeviceSurfaceInfo2KHR surface_info { .surface = surface };

        const auto present_modes { device.getSurfacePresentModesKHR(surface) };
        const auto surf_capabs { device.getSurfaceCapabilities2KHR(
          surface_info) };

        bool mailbox_found { std::find(present_modes.cbegin(),
                                       present_modes.cend(),
                                       vk::PresentModeKHR::eMailbox)
                             != present_modes.cend() };

        if ( mailbox_found
             && surf_capabs.surfaceCapabilities.minImageCount >= 3 ) {
            info.swapchain_info.present_mode = vk::PresentModeKHR::eMailbox;
        }
        else if ( surf_capabs.surfaceCapabilities.minImageCount >= 1 ) {
            info.swapchain_info.present_mode = vk::PresentModeKHR::eFifo;
        }
        else {
            can_present = false;
        }

        // Surface formats
        bool surface_complete { false };

        const auto surface_formats { device.getSurfaceFormats2KHR(
          surface_info) };

        for ( const auto& format : surface_formats ) {
            if ( format.surfaceFormat.colorSpace
                   == vk::ColorSpaceKHR::eSrgbNonlinear
                 && format.surfaceFormat.format == vk::Format::eB8G8R8A8Srgb )
            {
                info.swapchain_info.surface_format = vk::Format::eB8G8R8A8Srgb,
                info.swapchain_info.color_space =
                  vk::ColorSpaceKHR::eSrgbNonlinear;
                surface_complete = true;
            }
        }

        if ( can_present && surface_complete && info.queues.is_suitable() ) {
            return info;
        }
        else
            return {};
    }

    vk::SurfaceKHR create_surface(const vk::Instance& instance,
                                  GLFWwindow*         window_handle) {

        VkSurfaceKHR vksurface;
        if ( glfwCreateWindowSurface(instance,
                                     window_handle,
                                     nullptr,
                                     &vksurface)
             != VK_SUCCESS )
        {
            throw std::runtime_error("Cannot create surface");
        }
        return vk::SurfaceKHR(vksurface);
    }

#pragma endregion UTILS

}  // namespace potato::render
