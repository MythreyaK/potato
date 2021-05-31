#include "device.hpp"
#include "surface.hpp"

#include <set>

// Contains stuff related to device swapchain management

extern "C" {
    void glfwGetFramebufferSize(GLFWwindow*, int* w, int* h);
}

namespace potato::render {

    using attachments = std::vector<vk::ImageView>;

    void surface::create_swapchain() {

        std::set<uint32_t> _queues {
            potato_device.info().queues.graphics_inx.value(),
            potato_device.info().queues.present_inx.value()
        };

        std::vector<uint32_t> queues { _queues.begin(), _queues.end() };

        swapchain = potato_device.logical().createSwapchainKHR(
          swapchain_create_info(queues));
        create_swapchain_stuff();
    }

    void surface::recreate_swapchain() {

        std::set<uint32_t> _queues {
            potato_device.info().queues.graphics_inx.value(),
            potato_device.info().queues.present_inx.value()
        };

        std::vector<uint32_t> queues { _queues.begin(), _queues.end() };

        auto new_swapchain { potato_device.logical().createSwapchainKHR(
          swapchain_create_info(queues, swapchain)) };

        swapchain = std::move(new_swapchain);
        create_swapchain_stuff();
    }

    // creates swapimages, swapimage-views
    void surface::create_swapchain_stuff() {
        swapimages = potato_device.logical().getSwapchainImagesKHR(swapchain);
        swapimageviews.reserve(swapimages.size());

        // Get all image views
        for ( auto& i : swapimages ) {
            swapimageviews.emplace_back(potato_device.logical().createImageView({
              .image      = i,
              .viewType   = vk::ImageViewType::e2D,
              .format     = potato_device.info().swapchain.surface_format,
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
    surface::swapchain_create_info(const std::vector<uint32_t>& queues) const {

        constexpr auto exclusive  = vk::SharingMode::eExclusive;
        constexpr auto concurrent = vk::SharingMode::eConcurrent;

        return { .surface       = render_surface,
                 .minImageCount = potato_device.info().swapchain.image_count,
                 .imageFormat   = potato_device.info().swapchain.surface_format,
                 .imageColorSpace  = potato_device.info().swapchain.color_space,
                 .imageExtent      = current_extent(),
                 .imageArrayLayers = 1,  // 2 for streoscopic 3D
                 .imageUsage       = vk::ImageUsageFlagBits::eColorAttachment,
                 .imageSharingMode = queues.size() == 1 ? exclusive : concurrent,
                 .queueFamilyIndexCount = static_cast<uint32_t>(queues.size()),
                 .pQueueFamilyIndices   = queues.data(),
                 .preTransform          = current_transform(),
                 .compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque,
                 .presentMode    = potato_device.info().swapchain.present_mode,
                 .clipped        = true };
    }

    vk::SwapchainCreateInfoKHR surface::swapchain_create_info(
      const std::vector<uint32_t>& queues,
      const vk::SwapchainKHR&      old_swapchain) const {

        constexpr auto exclusive  = vk::SharingMode::eExclusive;
        constexpr auto concurrent = vk::SharingMode::eConcurrent;

        return { .surface       = render_surface,
                 .minImageCount = potato_device.info().swapchain.image_count,
                 .imageFormat   = potato_device.info().swapchain.surface_format,
                 .imageColorSpace  = potato_device.info().swapchain.color_space,
                 .imageExtent      = current_extent(),
                 .imageArrayLayers = 1,  // 2 for streoscopic 3D
                 .imageUsage       = vk::ImageUsageFlagBits::eColorAttachment,
                 .imageSharingMode = queues.size() == 1 ? exclusive : concurrent,
                 .queueFamilyIndexCount = static_cast<uint32_t>(queues.size()),
                 .pQueueFamilyIndices   = queues.data(),
                 .preTransform          = current_transform(),
                 .compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque,
                 .presentMode    = potato_device.info().swapchain.present_mode,
                 .clipped        = true,
                 .oldSwapchain   = old_swapchain };
    }

    // destroy swapimage-views
    void surface::destroy_swapchain_stuff() {
        for ( auto& i : swapimageviews )
            potato_device.logical().destroyImageView(i);
        swapimageviews.clear();
        swapimages.clear();
    }

}  // namespace potato::render
