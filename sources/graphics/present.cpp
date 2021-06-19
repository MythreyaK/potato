#include "device.hpp"
#include "surface.hpp"

#include <numeric>

namespace potato::render {

    void surface::acquire_image() {
        static constexpr auto tmax { std::numeric_limits<uint64_t>::max() };

        // wait for the fence for the current frame before trying to acquire it
        potato_device->logical().waitForFences(in_flight_fence[current_frame],
                                               true,
                                               tmax);

        auto result { potato_device->logical().acquireNextImageKHR(
          swapchain,                       // swapchain to acquire from
          tmax,                            // wait timeout
          image_available[current_frame],  // semaphore to signal
          {}                               // fences
          ) };

        if ( result.result == vk::Result::eSuccess ) [[likely]] {
            framebuffer_index = result.value;
            // Got an index, check if it's still in "in-flight" by checking
            // for its fence
            if ( in_flight_image[framebuffer_index] != vk::Fence {} ) {
                potato_device->logical().waitForFences(
                  in_flight_image[framebuffer_index],
                  true,
                  tmax);
            }

            // now we set the fence for this image as "in use"
            in_flight_image[framebuffer_index] = in_flight_fence[current_frame];
        }

        else [[unlikely]] {
            throw std::runtime_error("Failed to acquire swapimage");
        }
    }

    const vk::CommandBuffer&
    surface::begin_frame(const vk::RenderPass& renderpass,
                         const pipeline&       pipeline) {
        assert(!frame_in_progress && "Frame must be started");

        static std::array<vk::ClearValue, 2> clr_val {};

        clr_val[1].depthStencil = vk::ClearDepthStencilValue { 1.0f, 0 };

        clr_val[0].color =
          vk::ClearColorValue { std::array { 0.01f, 0.01f, 0.01f, 1.0f } };

        acquire_image();

        auto& cmd_buffer { current_cmd_buffer() };

        vk::CommandBufferBeginInfo cmd_buffer_begin_info {};

        cmd_buffer.begin(cmd_buffer_begin_info);

        vk::RenderPassBeginInfo renderpass_begin_info {
            .renderPass      = renderpass,
            .framebuffer     = framebuffers[framebuffer_index],
            .renderArea      = { .offset = {}, .extent = current_extent() },
            .clearValueCount = static_cast<uint32_t>(clr_val.size()),
            .pClearValues    = clr_val.data(),
        };

        cmd_buffer.beginRenderPass(renderpass_begin_info,
                                   vk::SubpassContents::eInline);
        pipeline.bind(cmd_buffer);
        return cmd_buffer;
    }

    void surface::end_frame() {
        auto& cmd_buffer { current_cmd_buffer() };
        cmd_buffer.endRenderPass();
        cmd_buffer.end();

        constexpr vk::PipelineStageFlags flags_mask {
            vk::PipelineStageFlagBits::eColorAttachmentOutput
        };

        vk::SubmitInfo submit_info {
            .waitSemaphoreCount   = 1,
            .pWaitSemaphores      = &image_available[current_frame],
            .pWaitDstStageMask    = &flags_mask,
            .commandBufferCount   = 1,
            .pCommandBuffers      = &cmd_buffer,
            .signalSemaphoreCount = 1,
            .pSignalSemaphores    = &render_complete[current_frame],
        };

        vk::PresentInfoKHR present_info {
            .waitSemaphoreCount = 1,
            .pWaitSemaphores    = &render_complete[current_frame],
            .swapchainCount     = 1,
            .pSwapchains        = &swapchain,
            .pImageIndices      = &framebuffer_index,
            .pResults           = {},
        };

        // get the graphics queue
        auto queue { potato_device->get_queues().at(
          vk::QueueFlagBits::eGraphics) };

        potato_device->logical().resetFences({ in_flight_fence[current_frame] });

        queue.submit(submit_info, in_flight_fence[current_frame]);

        queue.presentKHR(present_info);

        frame_in_progress = false;
        current_frame     = (current_frame + 1) % MAX_FRAMES_IN_FLIGHT;
    }

}  // namespace potato::render
