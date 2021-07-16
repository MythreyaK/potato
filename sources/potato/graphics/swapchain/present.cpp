#include "device/device.hpp"
#include "surface/surface.hpp"
#include "swapchain/swapchain.hpp"

#include <core/utils.hpp>
#include <numeric>

namespace potato::graphics {

    void swapchain::acquire_image() {
        static constexpr auto tmax { std::numeric_limits<uint64_t>::max() };

        // wait for the fence for the current frame before trying to acquire it
        m_device->logical->waitForFences(m_in_flight_fence[m_current_frame],
                                         true,
                                         tmax);

        auto [result, value] { m_device->logical->acquireNextImageKHR(
          m_swapchain,                         // swapchain to acquire from
          tmax,                                // wait timeout
          m_image_available[m_current_frame],  // semaphore to signal
          {}                                   // fences
          ) };

        if ( result == vk::Result::eSuccess ) [[likely]] {
            m_framebuffer_inx = value;
            // Got an index, check if it's still in "in-flight" by checking
            // for its fence
            if ( m_in_flight_image[m_framebuffer_inx] != vk::Fence {} ) {
                m_device->logical->waitForFences(
                  m_in_flight_image[m_framebuffer_inx],
                  true,
                  tmax);
            }

            // now we set the fence for this image as "in use"
            m_in_flight_image[m_framebuffer_inx] =
              m_in_flight_fence[m_current_frame];
        }

        else [[unlikely]] {
            throw std::runtime_error("Failed to acquire swapimage");
        }
    }

    const vk::CommandBuffer& swapchain::begin_frame() {
        static const vk::CommandBufferBeginInfo cmd_begin_info {};

        assert(!m_frame_in_progress && "Frame must be started");

        acquire_image();

        auto& cmd_buffer { current_cmd_buffer() };

        cmd_buffer.begin(&cmd_begin_info);

        return cmd_buffer;
    }

    void swapchain::begin_renderpass() {
        using namespace potato::utils;

        // clang-format off
        static std::array<vk::ClearValue, 2> clr_val { {
            { vk::ClearColorValue { std::array { 0.01f, 0.01f, 0.01f, 1.0f } } },
            { vk::ClearDepthStencilValue { 1.0f, 0 } }
        } };
        // clang-format on

        auto&      cmd_buffer { current_cmd_buffer() };
        const auto frame_extent { m_surface->framebuffer_size(
          m_device->physical) };

        cmd_buffer.setScissor(0, { { .extent = frame_extent } });
        cmd_buffer.setViewport(
          0,
          { { .width    = static_cast<float>(frame_extent.width),
              .height   = static_cast<float>(frame_extent.height),
              .minDepth = 0.0f,
              .maxDepth = 1.0f } });

        vk::RenderPassBeginInfo renderpass_begin_info {
            .renderPass      = m_renderpass,
            .framebuffer     = m_framebuffers[m_framebuffer_inx],
            .renderArea      = { .extent = frame_extent },
            .clearValueCount = vksize(clr_val),
            .pClearValues    = clr_val.data(),
        };

        cmd_buffer.beginRenderPass(renderpass_begin_info,
                                   vk::SubpassContents::eInline);
    }

    void swapchain::end_renderpass() {
        current_cmd_buffer().endRenderPass();
    }

    void swapchain::end_frame() {
        auto& cmd_buffer { current_cmd_buffer() };
        cmd_buffer.end();

        constexpr vk::PipelineStageFlags flags_mask {
            vk::PipelineStageFlagBits::eColorAttachmentOutput
        };

        vk::SubmitInfo submit_info {
            .waitSemaphoreCount   = 1,
            .pWaitSemaphores      = &m_image_available[m_current_frame],
            .pWaitDstStageMask    = &flags_mask,
            .commandBufferCount   = 1,
            .pCommandBuffers      = &cmd_buffer,
            .signalSemaphoreCount = 1,
            .pSignalSemaphores    = &m_render_complete[m_current_frame],
        };

        vk::PresentInfoKHR present_info {
            .waitSemaphoreCount = 1,
            .pWaitSemaphores    = &m_render_complete[m_current_frame],
            .swapchainCount     = 1,
            .pSwapchains        = &m_swapchain,
            .pImageIndices      = &m_framebuffer_inx,
            .pResults           = {},
        };

        // get the graphics queue
        auto queue { m_device->queues.at(vk::QueueFlagBits::eGraphics) };

        m_device->logical->resetFences({ m_in_flight_fence[m_current_frame] });

        queue.submit(submit_info, m_in_flight_fence[m_current_frame]);

        queue.presentKHR(present_info);

        m_frame_in_progress = false;
        m_current_frame     = (m_current_frame + 1) % MAX_FRAMES_IN_FLIGHT;
    }

}  // namespace potato::graphics
