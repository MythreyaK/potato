#include "render.hpp"

#include <format>
#include <iostream>
#include <vector>

namespace potato::render {

    render_instance::render_instance(GLFWwindow* window_handle)
      : window_handle(window_handle)
      , context(window_handle)
      , render_device(context.get_instance(), window_handle)
      , render_surface(window_handle, render_device) {

        create_command_buffers(render_device.info().queues.graphics_inx.value());
    }

    void render_instance::window_resized() {
        render_device.logical().waitIdle();
        render_surface.reinitialize();
    }

}  // namespace potato::render
