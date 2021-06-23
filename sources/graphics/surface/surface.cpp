#include "surface.hpp"

extern "C" {
    // TODO: Maybe replace with callbacks? std::function?
    void     glfwGetFramebufferSize(GLFWwindow*, int*, int*);
    VkResult glfwCreateWindowSurface(VkInstance,
                                     GLFWwindow*,
                                     const VkAllocationCallbacks*,
                                     VkSurfaceKHR*);
}

namespace potato::render {

    surface::surface(const vk::Instance& instance, GLFWwindow* wh) {

        window_handle = wh;
        VkSurfaceKHR c_vksurface;

        if ( glfwCreateWindowSurface(instance,
                                     window_handle,
                                     nullptr,
                                     &c_vksurface)
             != VK_SUCCESS )
        {
            throw std::runtime_error("Cannot create surface");
        }

        vk::ObjectDestroy<vk::Instance, vk::DispatchLoaderDynamic>
          surfaceDeleter { instance, nullptr, VULKAN_HPP_DEFAULT_DISPATCHER };

        vksurface =
          vk::UniqueSurfaceKHR(vk::SurfaceKHR(c_vksurface), surfaceDeleter);
    }

    std::shared_ptr<surface> surface::make_shared() {
        return shared_from_this();
    }

    std::shared_ptr<const surface> surface::make_shared() const {
        return shared_from_this();
    }

    const vk::SurfaceKHR& surface::get() const {
        return *vksurface;
    }

    vk::Extent2D
    surface::framebuffer_size(const vk::PhysicalDevice& device) const {
        const auto capabs {
            device.getSurfaceCapabilities2KHR({ .surface = *vksurface })
              .surfaceCapabilities
        };

        int fb_w, fb_h;

        glfwGetFramebufferSize(window_handle, &fb_w, &fb_h);

        uint32_t ext_w { static_cast<uint32_t>(fb_w) };
        uint32_t ext_h { static_cast<uint32_t>(fb_h) };

        const auto& max_ext { capabs.maxImageExtent };
        const auto& min_ext { capabs.minImageExtent };

        return { { std::clamp(ext_w, min_ext.width, max_ext.width) },
                 { std::clamp(ext_h, min_ext.height, max_ext.height) } };
    }

    bool surface::can_present(const vk::PhysicalDevice& dev,
                              uint32_t                  queue_inx) const {

        return dev.getSurfaceSupportKHR(queue_inx, *vksurface);
    }

    vk::PresentModeKHR
    surface::get_present_mode(const vk::PhysicalDevice& dev) const {
        const auto present_modes { dev.getSurfacePresentModesKHR(*vksurface) };

        bool mailbox_found { std::find(present_modes.cbegin(),
                                       present_modes.cend(),
                                       vk::PresentModeKHR::eMailbox)
                             != present_modes.cend() };

        if ( mailbox_found )
            return vk::PresentModeKHR::eMailbox;
        else
            return vk::PresentModeKHR::eFifo;
    }

    std::pair<vk::Format, vk::ColorSpaceKHR>
    surface::get_present_setting(const vk::PhysicalDevice& dev) const {

        // clang-format off
        const auto surface_formats {
            dev.getSurfaceFormats2KHR({ .surface = *vksurface })
        };

        for ( const auto& format : surface_formats ) {

            if ( format.surfaceFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear
              && format.surfaceFormat.format == vk::Format::eB8G8R8A8Srgb )
            {
                return {
                    vk::Format::eB8G8R8A8Srgb,
                    vk::ColorSpaceKHR::eSrgbNonlinear
                };
            }
        }
        // clang-format on

        // Nothing found, give up
        throw std::runtime_error("Could not pick surface properties");
    }

    // Try to get request_count number of swap images
    // Returns supported number of swap images
    uint32_t surface::swapimage_count(const vk::PhysicalDevice& dev,
                                      uint32_t req_count) const {

        // clang-format off
        const auto surf_capabs { dev
            .getSurfaceCapabilities2KHR({ .surface = *vksurface })
            .surfaceCapabilities
        };
        // clang-format off

        const auto min_count { surf_capabs.minImageCount };

        // If max_count is 0, no upper bound exists. Set it
        // to some high value to clamp
        const auto max_count {
            surf_capabs.maxImageCount == 0 ? 100 : surf_capabs.maxImageCount };

        return std::clamp(req_count, min_count, max_count);
    }

}  // namespace potato::render
