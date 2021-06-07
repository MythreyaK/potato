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
            potato_device->info().queues.graphics_inx.value(),
            potato_device->info().queues.present_inx.value()
        };

        std::vector<uint32_t> queues { _queues.begin(), _queues.end() };

        swapchain = potato_device->logical().createSwapchainKHR(
          swapchain_create_info(queues));
        create_swapchain_stuff();
    }

    void surface::recreate_swapchain() {
        destroy_swapchain_stuff();

        std::set<uint32_t> _queues {
            potato_device->info().queues.graphics_inx.value(),
            potato_device->info().queues.present_inx.value()
        };

        std::vector<uint32_t> queues { _queues.begin(), _queues.end() };

        auto new_swapchain { potato_device->logical().createSwapchainKHR(
          swapchain_create_info(queues, swapchain)) };

        swapchain = std::move(new_swapchain);
        create_swapchain_stuff();
    }

    // creates all images, views
    void surface::create_swapchain_stuff() {
        const auto extent_2d { current_extent() };

        swapimages = potato_device->logical().getSwapchainImagesKHR(swapchain);

        // swapimageviews.reserve(swapimages.size());
        // If emplacing, do not resize
        depthimagesmemory.resize(swapimage_count());

        // Set up all depth, swap images and their views
        for ( int i = 0; i < swapimage_count(); ++i ) {

            swapimageviews.emplace_back(potato_device->logical().createImageView({
              .image      = swapimages[i],
              .viewType   = vk::ImageViewType::e2D,
              .format     = potato_device->info().swapchain.surface_format,
              .components = {},  // defaults all to vk::ComponentSwizzle::eIdentity
              .subresourceRange = {
                .aspectMask     = vk::ImageAspectFlagBits::eColor,
                .baseMipLevel   = 0,
                .levelCount     = 1,
                .baseArrayLayer = 0,
                .layerCount     = 1,
              },
            }));

            vk::ImageCreateInfo depthimage_ci {
                .imageType   = vk::ImageType::e2D,
                .format      = depth_format(),
                .extent      = {
                    .width = extent_2d.width,
                    .height = extent_2d.height,
                    .depth = 1,
                },
                .mipLevels   = 1,
                .arrayLayers = 1,
                .samples     = vk::SampleCountFlagBits::e1,
                .tiling      = vk::ImageTiling::eOptimal,
                .usage       = vk::ImageUsageFlagBits::eDepthStencilAttachment,
                .sharingMode = vk::SharingMode::eExclusive,
                .initialLayout = vk::ImageLayout::eUndefined,
            };

            // clang-format off
            depthimages.emplace_back(
                potato_device->create_image(depthimage_ci,
                    vk::MemoryPropertyFlagBits::eDeviceLocal,
                    depthimagesmemory[i]));

            vk::ImageViewCreateInfo depthimageview_ci {
                .image    = depthimages[i],
                .viewType = vk::ImageViewType::e2D,
                .format   = depth_format(),
                .subresourceRange = {
                    .aspectMask     = vk::ImageAspectFlagBits::eDepth,
                    .baseMipLevel   = 0,
                    .levelCount     = 1,
                    .baseArrayLayer = 0,
                    .layerCount     = 1,
                }
            };

            depthimageviews.emplace_back(
                potato_device->logical()
                .createImageView(depthimageview_ci));
            // clang-format on
        }
    }

    uint32_t surface::swapimage_count() const {
        return swapimages.size();
    }

    vk::SwapchainCreateInfoKHR
    surface::swapchain_create_info(const std::vector<uint32_t>& queues) const {

        constexpr auto exclusive  = vk::SharingMode::eExclusive;
        constexpr auto concurrent = vk::SharingMode::eConcurrent;

        return { .surface       = potato_device->get_surface(),
                 .minImageCount = potato_device->info().swapchain.image_count,
                 .imageFormat = potato_device->info().swapchain.surface_format,
                 .imageColorSpace = potato_device->info().swapchain.color_space,
                 .imageExtent     = current_extent(),
                 .imageArrayLayers = 1,  // 2 for streoscopic 3D
                 .imageUsage       = vk::ImageUsageFlagBits::eColorAttachment,
                 .imageSharingMode = queues.size() == 1 ? exclusive : concurrent,
                 .queueFamilyIndexCount = static_cast<uint32_t>(queues.size()),
                 .pQueueFamilyIndices   = queues.data(),
                 .preTransform          = current_transform(),
                 .compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque,
                 .presentMode    = potato_device->info().swapchain.present_mode,
                 .clipped        = true };
    }

    vk::SwapchainCreateInfoKHR surface::swapchain_create_info(
      const std::vector<uint32_t>& queues,
      const vk::SwapchainKHR&      old_swapchain) const {

        constexpr auto exclusive  = vk::SharingMode::eExclusive;
        constexpr auto concurrent = vk::SharingMode::eConcurrent;

        return { .surface       = potato_device->get_surface(),
                 .minImageCount = potato_device->info().swapchain.image_count,
                 .imageFormat = potato_device->info().swapchain.surface_format,
                 .imageColorSpace = potato_device->info().swapchain.color_space,
                 .imageExtent     = current_extent(),
                 .imageArrayLayers = 1,  // 2 for streoscopic 3D
                 .imageUsage       = vk::ImageUsageFlagBits::eColorAttachment,
                 .imageSharingMode = queues.size() == 1 ? exclusive : concurrent,
                 .queueFamilyIndexCount = static_cast<uint32_t>(queues.size()),
                 .pQueueFamilyIndices   = queues.data(),
                 .preTransform          = current_transform(),
                 .compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque,
                 .presentMode    = potato_device->info().swapchain.present_mode,
                 .clipped        = true,
                 .oldSwapchain   = old_swapchain };
    }

    // destroy swapimage-views
    void surface::destroy_swapchain_stuff() {
        for ( int i = 0; i < swapimage_count(); ++i ) {
            potato_device->logical().destroyImageView(swapimageviews[i]);
            potato_device->logical().destroyImageView(depthimageviews[i]);
            // potato_device->logical().destroyImage(swapimages[i]);
            potato_device->logical().destroyImage(depthimages[i]);
            potato_device->logical().freeMemory(depthimagesmemory[i]);
        }

        swapimageviews.clear();
        depthimagesmemory.clear();
        depthimageviews.clear();
        depthimages.clear();
        swapimages.clear();
        destroy_framebuffers();
    }

    vk::Format surface::depth_format() const {
        return potato_device->find_supported_format(
          {
            vk::Format::eD32Sfloat,
            vk::Format::eD32SfloatS8Uint,
            vk::Format::eD24UnormS8Uint,
          },
          vk::ImageTiling::eOptimal,
          vk::FormatFeatureFlagBits::eDepthStencilAttachment);
    }

}  // namespace potato::render
