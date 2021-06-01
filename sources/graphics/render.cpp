#include "render.hpp"

#include <format>
#include <iostream>
#include <vector>

namespace potato::render {

    render_instance::render_instance(GLFWwindow* window_handle)
      : window_handle(window_handle)
      , context(window_handle)
      , render_device(
          std::make_shared<device>(context.get_instance(), window_handle))
      , render_surface(window_handle, render_device->make_shared()){

        create_command_buffers(
          render_device->info().queues.graphics_inx.value());
    }

    void render_instance::window_resized() {
        render_device->logical().waitIdle();
        render_surface.reinitialize();
    }

    render_instance::~render_instance() {
        render_device->logical().destroyCommandPool(cmd_pool);
        render_device->logical().freeCommandBuffers(cmd_pool, cmd_buffers);
    }

}  // namespace potato::render
