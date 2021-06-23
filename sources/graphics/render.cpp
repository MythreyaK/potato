#include "render.hpp"

#include "surface/surface.hpp"

#include <format>
#include <iostream>
#include <vector>

namespace potato::render {

    render_instance::render_instance(GLFWwindow* window_handle)
      : window_handle { window_handle }
      , potato_instance {}
      , potato_surface { std::make_shared<surface>(potato_instance.get(),
                                                   window_handle) }
      , potato_device { std::make_shared<device>(potato_instance.get(),
                                                 *potato_surface) }
      , potato_swapchain { potato_device->shared_from_this(),
                           potato_device->create_info,
                           potato_surface->shared_from_this() } {

        // ctor
    }

    void render_instance::create_pipeline() {

        const auto curr_extent { potato_surface->framebuffer_size(
          potato_device->physical) };

        // call virtual functions
        renderpass =
          create_renderpass(potato_device->logical.get(),
                            potato_device->create_info.surface_format,
                            potato_swapchain.depth_format());

        auto pipeline_layout { create_pipeline_layout(
          potato_device->logical.get()) };

        auto pipeline_create_info { create_pipeline_info() };

        // NOTE: Some items in default pipeline create info aren't set,
        // so we set them here as needed.

        // clang-format off
        pipeline_create_info.info.scissor = vk::Rect2D {
            .offset = {},
            .extent = curr_extent
        };

        pipeline_create_info.info.viewport = vk::Viewport {
            .x        = 0,
            .y        = 0,
            .width    = static_cast<float>(curr_extent.width),
            .height   = static_cast<float>(curr_extent.height),
            .minDepth = 0.0f,
            .maxDepth = 1.0f
        };
        // clang-format on

        potato_pipeline = pipeline(potato_device->shared_from_this(),
                                   pipeline_create_info,
                                   std::move(pipeline_layout),
                                   renderpass);

        potato_swapchain.create_framebuffers(renderpass);
    }

    void render_instance::window_resized() {
        potato_device->logical->waitIdle();
        potato_swapchain.recreate_swapchain();
        potato_device->logical->destroyRenderPass(renderpass);
        create_pipeline();
    }

    render_instance::~render_instance() {
        potato_device->logical->waitIdle();
        potato_device->logical->destroyRenderPass(renderpass);
    }

    const pipeline& render_instance::get_pipeline() const {
        return potato_pipeline;
    }

    const vk::RenderPass& render_instance::get_renderpass() const {
        return renderpass;
    }

    const surface& render_instance::get_surface() const {
        return *potato_surface;
    }

    swapchain& render_instance::get_swapchain() {
        return potato_swapchain;
    }

    std::shared_ptr<const device> render_instance::get_device() const {
        return potato_device->shared_from_this();
    }

}  // namespace potato::render
