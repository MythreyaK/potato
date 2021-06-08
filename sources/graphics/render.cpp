#include "render.hpp"

#include <format>
#include <iostream>
#include <vector>

namespace potato::render {

    render_instance::render_instance(GLFWwindow* window_handle)
      : window_handle(window_handle)
      , potato_context(window_handle)
      , potato_device(std::make_shared<device>(potato_context.get_instance(),
                                               window_handle))
      , potato_surface(window_handle, potato_device->make_shared()) {

        // ctor
    }

    void render_instance::create_pipeline() {

        const auto curr_extent { potato_surface.current_extent() };

        // call virtual functions
        renderpass =
          create_renderpass(potato_device->logical(),
                            potato_device->info().swapchain.surface_format,
                            potato_surface.depth_format());

        auto pipeline_layout { create_pipeline_layout(
          potato_device->logical()) };

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

        potato_pipeline = pipeline(potato_device->make_shared(),
                                   pipeline_create_info,
                                   std::move(pipeline_layout),
                                   renderpass);

        potato_surface.create_framebuffers(renderpass);
    }

    void render_instance::window_resized() {
        potato_device->logical().waitIdle();
        potato_surface.recreate_swapchain();
        potato_device->logical().destroyRenderPass(renderpass);
        create_pipeline();
    }

    uint32_t render_instance::swapimage_count() const {
        return potato_surface.swapimage_count();
    }

    render_instance::~render_instance() {
        potato_device->logical().waitIdle();
        potato_device->logical().destroyRenderPass(renderpass);
    }

    const pipeline& render_instance::get_pipeline() const {
        return potato_pipeline;
    }

    const vk::RenderPass& render_instance::get_renderpass() const {
        return renderpass;
    }

    surface& render_instance::get_surface() {
        return potato_surface;
    }

    std::shared_ptr<const device> render_instance::get_device() const {
        return potato_device->make_shared();
    }

}  // namespace potato::render
