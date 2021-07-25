module potato.graphics : swapchain;

import : device;

namespace potato::graphics {

    void swapchain::create_sync_objects() {
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

    void swapchain::destroy_sync_objects() {
        for ( int i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i ) {
            m_device->logical->destroySemaphore(m_image_available[i]);
            m_device->logical->destroySemaphore(m_render_complete[i]);
            m_device->logical->destroyFence(m_in_flight_fence[i]);
        }

        m_image_available.clear();
        m_render_complete.clear();
        m_in_flight_fence.clear();
        m_in_flight_image.clear();
    }
}  // namespace potato::graphics
