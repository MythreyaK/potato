#include "device.hpp"

#include "utils.hpp"

#include <format>
#include <iostream>
#include <set>
#include <utility>

extern "C" {
    // TODO: Maybe replace with callbacks? std::function?

    VkResult glfwCreateWindowSurface(VkInstance,
                                     GLFWwindow*,
                                     const VkAllocationCallbacks*,
                                     VkSurfaceKHR*);
}

namespace potato::render {
    using namespace potato::render::detail;

#pragma region FWD_DECLARE

    const std::vector<std::string> required_device_extensions {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    vk::Device      create_device(device_settings device);
    device_settings pick_device(const vk::Instance&, const vk::SurfaceKHR&);
    vk::SurfaceKHR  create_surface(const vk::Instance&, GLFWwindow*);
    vkqueues get_queues(const vk::Device& dev, const device_settings& inf);
    std::optional<device_settings> get_suitable_device(const vk::PhysicalDevice&,
                                                       const vk::SurfaceKHR&);

#pragma endregion FWD_DECLARE

    /**** device class ****/

    device::device(const vk::Instance& instance, GLFWwindow* window_handle)
      : window_handle(window_handle)
      , instance(instance) {

        surface         = create_surface(instance, window_handle);
        device_info     = pick_device(instance, surface);
        physical_device = device_info.device;
        logical_device  = create_device(device_info);

        // Init device-specific pointers
        VULKAN_HPP_DEFAULT_DISPATCHER.init(logical_device);

        queues = get_queues(logical_device, device_info);
    }

    const device_settings& device::info() const {
        return device_info;
    }

    const vk::Device& device::logical() const {
        return logical_device;
    }
    const vk::PhysicalDevice& device::physical() const {
        return physical_device;
    }

    const vk::SurfaceKHR& device::get_surface() const {
        return surface;
    }

    device::~device() {
        instance.destroySurfaceKHR(surface);
        logical_device.waitIdle();
        logical_device.destroy();
    }

#pragma region UTILS
    vkqueues get_queues(const vk::Device& dev, const device_settings& inf) {
        // TODO: Update to be more flexible, use queues as needed
        auto gf = dev.getQueue2(vk::DeviceQueueInfo2 {
          .queueFamilyIndex = inf.queues.graphics_inx.value(),
          .queueIndex       = 0 });

        return { { vk::QueueFlagBits::eGraphics, gf } };
    }

    vk::Device create_device(device_settings device_info) {

        std::set<uint32_t> queues { device_info.queues.graphics_inx.value(),
                                    device_info.queues.present_inx.value() };

        // TODO: Ensure we have only one queue for now.
        if ( queues.size() != 1 ) {
            throw std::runtime_error(
              "Graphics queue does not support presentation");
        }

        std::vector<vk::DeviceQueueCreateInfo> q_create_infos {};

        const auto queuePriority = 1.0f;
        for ( auto queue : queues ) {

            q_create_infos.emplace_back(vk::DeviceQueueCreateInfo {
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
          .queueCreateInfoCount  = static_cast<uint32_t>(q_create_infos.size()),
          .pQueueCreateInfos     = q_create_infos.data(),
          .enabledExtensionCount = static_cast<uint32_t>(extns.size()),
          .ppEnabledExtensionNames = extns.data(),
          .pEnabledFeatures        = {},
        } };

        return device_info.device.createDevice(create_info);
    }

    device_settings pick_device(const vk::Instance&   inst,
                                const vk::SurfaceKHR& srf) {

        auto devices { inst.enumeratePhysicalDevices() };

        std::vector<device_settings> suitable_devices {};

        for ( auto& d : devices ) {
            auto info { get_suitable_device(d, srf) };
            if ( info.has_value() ) {
                suitable_devices.emplace_back(info.value());
            }
        }

        if ( suitable_devices.size() == 0 ) {
            throw std::runtime_error("Failed to find suitable gpu");
        }
        else {
            std::cout << std::format("Picked [{}]\n", suitable_devices[0].name);
            return suitable_devices[0];
        }
    }

    std::optional<device_settings>
    get_suitable_device(const vk::PhysicalDevice& device,
                        const vk::SurfaceKHR&     surface) {

        using qfb        = vk::QueueFlagBits;
        using vkdevprops = vk::PhysicalDeviceProperties2;
        using vkdvrprops = vk::PhysicalDeviceDriverProperties;

        device_settings info {};

        const auto all_props { device.getProperties2<vkdevprops, vkdvrprops>() };
        const auto& dev_props { all_props.get<vkdevprops>().properties };
        const auto& drv_props { all_props.get<vkdvrprops>() };

        const auto features { device.getFeatures2() };
        const auto queues { device.getQueueFamilyProperties2() };

        // device
        info.device_type = dev_props.deviceType;
        info.device      = device;
        info.name        = std::format("{}", dev_props.deviceName);

        // clang-format off
        std::cout << std::format(\
            "Device Name: {}      \n"
            "  Type:           {}\n"
            "  Texture limit:  {}\n"
            "  Driver name:    {}\n"
            "  Driver make:    {}\n"
            "  Driver version: {}\n"
            "  Vulkan version: {}\n"
            ,
            dev_props.deviceName                ,
            vk::to_string(dev_props.deviceType) ,
            dev_props.limits.maxImageDimension2D,
            drv_props.driverName                ,
            vk::to_string(drv_props.driverID)   ,
            drv_props.driverInfo                ,
            vk::to_string(drv_props.conformanceVersion)
        );
        // clang-format on

        // Get the queues
        uint32_t i { std::numeric_limits<uint32_t>::max() };
        for ( const auto& queue : queues ) {
            const auto props { queue.queueFamilyProperties };
            ++i;

            // clang-format off
            std::cout << std::format(
                "  Queue family {}\n"
                "    Flags: {}\n"
                "    Count: {}\n",
                i,
                vk::to_string(props.queueFlags),
                props.queueCount
            );
            // clang-format on

            // If the queues we need have been found, just log
            if ( info.queues.is_suitable() ) continue;

            if ( props.queueFlags & qfb::eGraphics
                 && !info.queues.graphics_inx.has_value() )
            {
                info.queues.graphics_inx = i;
            }

            if ( device.getSurfaceSupportKHR(i, surface) ) {
                info.queues.present_inx = i;
            }
        }

        // get extensions
        const auto device_extensions {
            device.enumerateDeviceExtensionProperties()
        };
        std::vector<std::string> supported_extns {};
        for ( const auto ext : device_extensions ) {
            supported_extns.push_back(ext.extensionName);
        }

        if ( !has_required_items("Device extensions",
                                 supported_extns,
                                 required_device_extensions) )
        {
            return {};
        }

        // Get the swapchain info
        bool can_present { true };

        vk::PhysicalDeviceSurfaceInfo2KHR surface_info { .surface = surface };

        const auto present_modes { device.getSurfacePresentModesKHR(surface) };
        const auto surf_capabs { device.getSurfaceCapabilities2KHR(
          surface_info) };

        const auto min_image { surf_capabs.surfaceCapabilities.minImageCount };
        const auto max_image { surf_capabs.surfaceCapabilities.maxImageCount };

        bool mailbox_found { std::find(present_modes.cbegin(),
                                       present_modes.cend(),
                                       vk::PresentModeKHR::eMailbox)
                             != present_modes.cend() };

        // swapchain image count, present modes
        if ( max_image == 0 || max_image >= 3 ) {
            // max_image eq 0 means no upper bound
            info.swapchain.image_count = std::max<uint32_t>(min_image, 3);
        }
        else {
            // max-image is either 1 or 2, since least value of min_image is 1
            info.swapchain.image_count = std::max(min_image, max_image);
        }

        if ( mailbox_found )
            info.swapchain.present_mode = vk::PresentModeKHR::eMailbox;
        else
            info.swapchain.present_mode = vk::PresentModeKHR::eFifo;

        // Surface formats
        bool surface_complete { false };

        const auto surface_formats { device.getSurfaceFormats2KHR(
          surface_info) };

        for ( const auto& format : surface_formats ) {
            if ( format.surfaceFormat.colorSpace
                   == vk::ColorSpaceKHR::eSrgbNonlinear
                 && format.surfaceFormat.format == vk::Format::eB8G8R8A8Srgb )
            {
                info.swapchain.surface_format = vk::Format::eB8G8R8A8Srgb,
                info.swapchain.color_space = vk::ColorSpaceKHR::eSrgbNonlinear;
                surface_complete           = true;
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
        if ( glfwCreateWindowSurface(instance, window_handle, nullptr, &vksurface)
             != VK_SUCCESS )
        {
            throw std::runtime_error("Cannot create surface");
        }
        return vk::SurfaceKHR(vksurface);
    }

#pragma endregion UTILS

}  // namespace potato::render
