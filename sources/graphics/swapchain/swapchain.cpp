#include "swapchain/swapchain.hpp"

#include "device/device.hpp"
#include "surface/surface.hpp"

#include <set>

// Contains stuff related to device swapchain management

extern "C" {
    void glfwGetFramebufferSize(GLFWwindow*, int* w, int* h);
}

namespace potato::graphics {

    using attachments = std::vector<vk::ImageView>;

    swapchain::swapchain(std::shared_ptr<const device>  device,
                         device_create_info             inf,
                         std::shared_ptr<const surface> surf)
      : m_device { device }
      , m_device_info { inf }
      , m_surface { std::move(surf) } {
        create_swapchain();
    }

    swapchain::~swapchain() {
        m_device->logical->destroySwapchainKHR(m_swapchain);
        destroy_swapchain_stuff();
    }

    vk::SurfaceTransformFlagBitsKHR swapchain::current_transform() const {
        return m_device->physical
          .getSurfaceCapabilities2KHR({ .surface = m_surface->get() })
          .surfaceCapabilities.currentTransform;
    }

    void swapchain::create_framebuffers(const vk::RenderPass& renderpass) {

        const auto extents { m_surface->framebuffer_size(m_device->physical) };

        vk::FramebufferCreateInfo framebuffer_ci {
            .renderPass      = renderpass,
            .attachmentCount = 2u,
            .width           = extents.width,
            .height          = extents.height,
            .layers          = 1,  // Q: same as swapchain layers?
        };

        m_framebuffers.reserve(m_swapimages.size());

        for ( int i = 0; i < swapimage_count(); ++i ) {
            std::array<vk::ImageView, 2> framebuffer_attachments {
                m_swapimageviews[i],
                m_depthimageviews[i]
            };

            framebuffer_ci.pAttachments = framebuffer_attachments.data();
            m_framebuffers.emplace_back(
              m_device->logical->createFramebuffer(framebuffer_ci));
        }
    }

    void swapchain::destroy_framebuffers() {
        for ( auto& fb : m_framebuffers ) {
            m_device->logical->destroyFramebuffer(fb);
        }
        m_framebuffers.clear();
    }

    void swapchain::create_swapchain() {

        std::vector<uint32_t> queues {
            m_device_info.q_families.graphics.value()
        };

        m_swapchain =
          m_device->logical->createSwapchainKHR(swapchain_create_info(queues));
        create_swapchain_stuff();
    }

    void swapchain::recreate_swapchain() {
        destroy_swapchain_stuff();

        std::vector<uint32_t> queues {
            m_device_info.q_families.graphics.value()
        };

        auto new_swapchain { m_device->logical->createSwapchainKHR(
          swapchain_create_info(queues, m_swapchain)) };

        m_swapchain = std::move(new_swapchain);
        create_swapchain_stuff();
    }

    // creates all images, views
    void swapchain::create_swapchain_stuff() {
        const auto extent_2d { m_surface->framebuffer_size(m_device->physical) };

        m_swapimages = m_device->logical->getSwapchainImagesKHR(m_swapchain);

        // swapimageviews.reserve(swapimages.size());
        // If emplacing, do not resize
        m_depthimagesmemory.resize(swapimage_count());

        // Set up all depth, swap images and their views
        for ( int i = 0; i < swapimage_count(); ++i ) {

            m_swapimageviews.emplace_back(m_device->logical->createImageView({
              .image      = m_swapimages[i],
              .viewType   = vk::ImageViewType::e2D,
              .format     = m_device_info.surface_format,
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
            m_depthimages.emplace_back(
                m_device->create_image(depthimage_ci,
                    vk::MemoryPropertyFlagBits::eDeviceLocal,
                    m_depthimagesmemory[i]));

            vk::ImageViewCreateInfo depthimageview_ci {
                .image    = m_depthimages[i],
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

            m_depthimageviews.emplace_back(
                m_device->logical->createImageView(depthimageview_ci));
            // clang-format on
        }

        create_command_buffers(m_device_info.q_families.graphics.value());

        for ( int i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i ) {
            m_image_available.emplace_back(
              m_device->logical->createSemaphore({}));

            m_render_complete.emplace_back(
              m_device->logical->createSemaphore({}));

            m_in_flight_fence.emplace_back(m_device->logical->createFence({
              .flags = vk::FenceCreateFlagBits::eSignaled,
            }));
        }

        m_in_flight_image.resize(swapimage_count(), {});
    }

    uint32_t swapchain::swapimage_count() const {
        return m_swapimages.size();
    }

    vk::SwapchainCreateInfoKHR swapchain::swapchain_create_info(
      const std::vector<uint32_t>& queues) const {

        constexpr auto exclusive  = vk::SharingMode::eExclusive;
        constexpr auto concurrent = vk::SharingMode::eConcurrent;

        return { .surface         = m_surface->get(),
                 .minImageCount   = m_device_info.image_count,
                 .imageFormat     = m_device_info.surface_format,
                 .imageColorSpace = m_device_info.color_space,
                 .imageExtent = m_surface->framebuffer_size(m_device->physical),
                 .imageArrayLayers = 1,  // 2 for streoscopic 3D
                 .imageUsage       = vk::ImageUsageFlagBits::eColorAttachment,
                 .imageSharingMode = queues.size() == 1 ? exclusive : concurrent,
                 .queueFamilyIndexCount = static_cast<uint32_t>(queues.size()),
                 .pQueueFamilyIndices   = queues.data(),
                 .preTransform          = current_transform(),
                 .compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque,
                 .presentMode    = m_device_info.present_mode,
                 .clipped        = true };
    }

    vk::SwapchainCreateInfoKHR swapchain::swapchain_create_info(
      const std::vector<uint32_t>& queues,
      const vk::SwapchainKHR&      old_swapchain) const {

        auto info { swapchain_create_info(queues) };
        info.oldSwapchain = old_swapchain;

        return info;
    }

    // destroy swapimage-views
    void swapchain::destroy_swapchain_stuff() {
        for ( int i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i ) {
            m_device->logical->destroySemaphore(m_image_available[i]);
            m_device->logical->destroySemaphore(m_render_complete[i]);
            m_device->logical->destroyFence(m_in_flight_fence[i]);
        }

        m_image_available.clear();
        m_render_complete.clear();
        m_in_flight_fence.clear();
        m_in_flight_image.clear();

        m_device->logical->freeCommandBuffers(m_cmd_pool, m_cmd_buffers);
        m_device->logical->destroyCommandPool(m_cmd_pool);

        destroy_framebuffers();

        for ( int i = 0; i < swapimage_count(); ++i ) {
            m_device->logical->destroyImageView(m_swapimageviews[i]);
            m_device->logical->destroyImageView(m_depthimageviews[i]);
            // m_device->logical->destroyImage(swapimages[i]);
            m_device->logical->destroyImage(m_depthimages[i]);
            m_device->logical->freeMemory(m_depthimagesmemory[i]);
        }

        m_swapimageviews.clear();
        m_depthimagesmemory.clear();
        m_depthimageviews.clear();
        m_depthimages.clear();
        m_swapimages.clear();
    }

    vk::Format swapchain::depth_format() const {
        return m_device->find_supported_format(
          {
            vk::Format::eD32Sfloat,
            vk::Format::eD32SfloatS8Uint,
            vk::Format::eD24UnormS8Uint,
          },
          vk::ImageTiling::eOptimal,
          vk::FormatFeatureFlagBits::eDepthStencilAttachment);
    }

}  // namespace potato::graphics
