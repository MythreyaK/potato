#include "render.hpp"

#include "surface/surface.hpp"

#include <format>
#include <iostream>
#include <vector>

namespace potato::graphics {

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

    void render_instance::window_resized() {
        potato_device->logical->waitIdle();
        potato_swapchain.recreate_swapchain();
    }

    render_instance::~render_instance() {
        potato_device->logical->waitIdle();
    }

    const surface& render_instance::get_surface() const {
        return *potato_surface;
    }

    swapchain& render_instance::get_swapchain() {
        return potato_swapchain;
    }

    const device& render_instance::get_device() const {
        return *potato_device;
    }

}  // namespace potato::graphics
