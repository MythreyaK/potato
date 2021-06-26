#include "device.hpp"

#include "surface/surface.hpp"
#include "utils/utils.hpp"

#include <format>
#include <iostream>
#include <set>
#include <utility>

namespace potato::graphics {

#pragma region FWD_DECLARE

    using queue_famiies = std::map<vk::QueueFlagBits, uint32_t>;

    const std::vector<std::string> required_device_extensions {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    };

    vk::UniqueDevice   create_device(device_create_info);
    device_create_info pick_device(const vk::Instance&, const surface&);
    vkqueues           get_queues(const vk::Device&, const device_create_info&);

    std::optional<device_create_info>
    get_suitable_device(const vk::PhysicalDevice&, const surface&);

#pragma endregion FWD_DECLARE

    /**** device class ****/

    device::device(const vk::Instance& instance, const surface& surface)
      : create_info { pick_device(instance, surface) }
      , physical { create_info.device }
      , logical { create_device(create_info) }
      , queues { get_queues(*logical, create_info) } {}

    device::~device() {
        logical->waitIdle();
    }

#pragma region UTILS
    vkqueues get_queues(const vk::Device&         dev,
                        const device_create_info& queues) {

        // TODO: Update to be more flexible, use queues as needed
        auto gf = dev.getQueue2(vk::DeviceQueueInfo2 {
          .queueFamilyIndex = queues.q_families.graphics.value(),
          .queueIndex       = 0 });

        return { { vk::QueueFlagBits::eGraphics, gf } };
    }

    vk::UniqueDevice create_device(device_create_info device_info) {

        std::set<uint32_t> queues { device_info.q_families.graphics.value(),
                                    device_info.q_families.present.value() };

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

        auto ret { device_info.device.createDeviceUnique(create_info) };

        // Init device-specific pointers
        VULKAN_HPP_DEFAULT_DISPATCHER.init(*ret);

        return std::move(ret);
    }

    device_create_info pick_device(const vk::Instance& inst,
                                   const surface&      srf) {

        auto devices { inst.enumeratePhysicalDevices() };

        std::vector<device_create_info> suitable_devices {};

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
            std::cout << std::format("Picked [{}]\n",
                                     suitable_devices[0].device_name);
            return suitable_devices[0];
        }
    }

    std::optional<device_create_info>
    get_suitable_device(const vk::PhysicalDevice& device,
                        const surface&            surface) {

        using namespace potato::utils;
        using qfb        = vk::QueueFlagBits;
        using vkdevprops = vk::PhysicalDeviceProperties2;
        using vkdvrprops = vk::PhysicalDeviceDriverProperties;

        device_create_info info {};

        const auto all_props { device.getProperties2<vkdevprops, vkdvrprops>() };
        const auto& dev_props { all_props.get<vkdevprops>().properties };
        const auto& drv_props { all_props.get<vkdvrprops>() };

        const auto features { device.getFeatures2() };
        const auto queues { device.getQueueFamilyProperties2() };
        const auto surface_settings { surface.get_present_setting(device) };

        info.device         = device;
        info.device_name    = std::format("{}", dev_props.deviceName);
        info.present_mode   = surface.get_present_mode(device);
        info.color_space    = surface_settings.second;
        info.surface_format = surface_settings.first;
        info.image_count    = surface.swapimage_count(device, 3);

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
            if ( info.q_families.is_suitable() ) continue;

            if ( props.queueFlags & qfb::eGraphics
                 && !info.q_families.graphics.has_value() )
            {
                info.q_families.graphics = i;
            }

            if ( surface.can_present(device, i) ) {
                info.q_families.present = i;
            }
        }

        // get extensions
        const auto device_extensions {
            device.enumerateDeviceExtensionProperties()
        };

        std::vector<std::string> supported_extns {};
        for ( const auto& ext : device_extensions ) {
            supported_extns.push_back(ext.extensionName);
        }

        if ( !has_required_items("Device extensions",
                                 supported_extns,
                                 required_device_extensions) )
        {
            return {};
        }

        if ( info.q_families.is_suitable() ) {
            return info;
        }
        else
            return {};
    }

#pragma endregion UTILS

}  // namespace potato::graphics
