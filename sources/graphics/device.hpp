#ifndef POTATO_RENDER_DEVICE_HPP
#define POTATO_RENDER_DEVICE_HPP

#include "vkinclude/vulkan.hpp"

extern "C" {
    struct GLFWwindow;
}

namespace potato::render {

    class device {

      private:
        const vk::Instance& instance {};
        vk::SurfaceKHR      surface {};
        vk::PhysicalDevice  physical_device {};
        vk::UniqueDevice    logical_device {};

      public:
        device(const vk::Instance& instance, GLFWwindow* window_handle);
        ~device();

        // no copies
        device(const device&) = delete;
        device& operator=(const device&) = delete;
    };

}  // namespace potato::render

#endif  // !POTATO_RENDER_DEVICE_HPP
