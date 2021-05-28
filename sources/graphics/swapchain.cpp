#include "device.hpp"

#include <set>

// Contains stuff related to device swapchain management

extern "C" {
    void glfwGetFramebufferSize(GLFWwindow*, int* w, int* h);
}

namespace potato::render {

    using attachments = std::vector<vk::ImageView>;

    void device::create_swapchain() {

        std::set<uint32_t> _queues { device_info.queues.graphics_inx.value(),
                                     device_info.queues.present_inx.value() };

        std::vector<uint32_t> queues { _queues.begin(), _queues.end() };

        swapchain =
          logical_device->createSwapchainKHR(swapchain_create_info(queues));
        create_swapchain_stuff();
    }

    void device::recreate_swapchain() {

        std::set<uint32_t> _queues { device_info.queues.graphics_inx.value(),
                                     device_info.queues.present_inx.value() };

        std::vector<uint32_t> queues { _queues.begin(), _queues.end() };

        auto new_swapchain { logical_device->createSwapchainKHR(
          swapchain_create_info(queues, swapchain)) };

        swapchain = new_swapchain;
        create_swapchain_stuff();
    }

    // creates swapimages, swapimage-views
    void device::create_swapchain_stuff() {
        swapimages = logical_device->getSwapchainImagesKHR(swapchain);
        swapimageviews.reserve(swapimages.size());

        // Get all image views
        for ( auto& i : swapimages ) {
            swapimageviews.emplace_back(logical_device->createImageView({
              .image      = i,
              .viewType   = vk::ImageViewType::e2D,
              .format     = device_info.swapchain.surface_format,
              .components = {},  // defaults all to vk::ComponentSwizzle::eIdentity
              .subresourceRange = {
                .aspectMask     = vk::ImageAspectFlagBits::eColor,
                .baseMipLevel   = 0,
                .levelCount     = 1,
                .baseArrayLayer = 0,
                .layerCount     = 1,
              },
            }));
        }
    }

    vk::SwapchainCreateInfoKHR
    device::swapchain_create_info(const std::vector<uint32_t>& queues) const {

        constexpr auto exclusive  = vk::SharingMode::eExclusive;
        constexpr auto concurrent = vk::SharingMode::eConcurrent;

        return { .surface          = surface,
                 .minImageCount    = device_info.swapchain.image_count,
                 .imageFormat      = device_info.swapchain.surface_format,
                 .imageColorSpace  = device_info.swapchain.color_space,
                 .imageExtent      = current_extent(),
                 .imageArrayLayers = 1,  // 2 for streoscopic 3D
                 .imageUsage       = vk::ImageUsageFlagBits::eColorAttachment,
                 .imageSharingMode = queues.size() == 1 ? exclusive : concurrent,
                 .queueFamilyIndexCount = static_cast<uint32_t>(queues.size()),
                 .pQueueFamilyIndices   = queues.data(),
                 .preTransform          = current_transform(),
                 .compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque,
                 .presentMode    = device_info.swapchain.present_mode,
                 .clipped        = true };
    }

    vk::SwapchainCreateInfoKHR
    device::swapchain_create_info(const std::vector<uint32_t>& queues,
                                  const vk::SwapchainKHR& old_swapchain) const {

        constexpr auto exclusive  = vk::SharingMode::eExclusive;
        constexpr auto concurrent = vk::SharingMode::eConcurrent;

        return { .surface          = surface,
                 .minImageCount    = device_info.swapchain.image_count,
                 .imageFormat      = device_info.swapchain.surface_format,
                 .imageColorSpace  = device_info.swapchain.color_space,
                 .imageExtent      = current_extent(),
                 .imageArrayLayers = 1,  // 2 for streoscopic 3D
                 .imageUsage       = vk::ImageUsageFlagBits::eColorAttachment,
                 .imageSharingMode = queues.size() == 1 ? exclusive : concurrent,
                 .queueFamilyIndexCount = static_cast<uint32_t>(queues.size()),
                 .pQueueFamilyIndices   = queues.data(),
                 .preTransform          = current_transform(),
                 .compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque,
                 .presentMode    = device_info.swapchain.present_mode,
                 .clipped        = true,
                 .oldSwapchain   = old_swapchain };
    }

    vk::Extent2D device::current_extent() const {
        const auto capabs {
            physical_device.getSurfaceCapabilities2KHR({ .surface = surface })
              .surfaceCapabilities
        };

        int fb_w, fb_h;
        glfwGetFramebufferSize(window_handle, &fb_w, &fb_h);

        uint32_t ext_w { static_cast<uint32_t>(fb_w) };
        uint32_t ext_h { static_cast<uint32_t>(fb_h) };

        const auto& max_ext { capabs.maxImageExtent };
        const auto& min_ext { capabs.minImageExtent };

        const auto& extent_w { std::clamp(ext_w, min_ext.width, max_ext.width) };
        const auto& extent_h {
            std::clamp(ext_h, min_ext.height, max_ext.height)
        };

        return { ext_w, ext_h };
    }

    vk::SurfaceTransformFlagBitsKHR device::current_transform() const {
        return physical_device
          .getSurfaceCapabilities2KHR({ .surface = surface })
          .surfaceCapabilities.currentTransform;
    }

    // destroy swapimage-views
    void device::destroy_swapchain_stuff() {
        for ( auto& i : swapimageviews )
            logical_device->destroyImageView(i);
    }

    void device::create_framebuffers(const vk::RenderPass& renderpass) {

        const auto extents { current_extent() };

        vk::FramebufferCreateInfo framebuffer_ci {
            .renderPass      = renderpass,
            .attachmentCount = 1u,
            .width           = extents.width,
            .height          = extents.height,
            .layers          = 1,  // Q: same as swapchain layers?
        };

        framebuffers.reserve(swapimages.size());
        for ( const auto& swp_img : swapimageviews ) {
            framebuffer_ci.pAttachments = &swp_img;
            framebuffers.emplace_back(
              logical_device->createFramebuffer(framebuffer_ci));
        }
    }

    void device::destroy_framebuffers() {
        for ( auto& fb : framebuffers ) {
            logical_device->destroyFramebuffer(fb);
        }
    }

}  // namespace potato::render
