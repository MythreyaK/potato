#include "surface.hpp"

#include "device.hpp"

#include <set>

extern "C" {
    void glfwGetFramebufferSize(GLFWwindow*, int*, int*);
}

namespace potato::render {

    surface::surface(GLFWwindow* window, std::shared_ptr<const device> device)
      : window_handle(window)
      , potato_device(device) {
        create_swapchain();
    }

    vk::Extent2D surface::current_extent() const {
        const auto capabs { potato_device->physical()
                              .getSurfaceCapabilities2KHR(
                                { .surface = potato_device->get_surface() })
                              .surfaceCapabilities };

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

    surface::~surface() {
        destroy_swapchain_stuff();
        potato_device->logical().destroySwapchainKHR(swapchain);
    }

    vk::SurfaceTransformFlagBitsKHR surface::current_transform() const {
        return potato_device->physical()
          .getSurfaceCapabilities2KHR(
            { .surface = potato_device->get_surface() })
          .surfaceCapabilities.currentTransform;
    }

    void surface::create_framebuffers(const vk::RenderPass& renderpass) {

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
              potato_device->logical().createFramebuffer(framebuffer_ci));
        }
    }

    void surface::destroy_framebuffers() {
        for ( auto& fb : framebuffers ) {
            potato_device->logical().destroyFramebuffer(fb);
        }
    }

}  // namespace potato::render
