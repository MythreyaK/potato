#ifndef POTATO_RENDER_HPP
#define POTATO_RENDER_HPP

// clang-format off
// #include "vkinclude/vulkan.hpp"
// clang-format on
#include "device/device.hpp"
#include "instance.hpp"
#include "pipeline.hpp"
#include "surface/surface.hpp"
#include "swapchain/swapchain.hpp"

#include <vector>

namespace potato::graphics {

    class render_instance {
      private:
        GLFWwindow*              window_handle;
        instance                 potato_instance;
        std::shared_ptr<surface> potato_surface;
        std::shared_ptr<device>  potato_device;
        swapchain                potato_swapchain;

      public:
        render_instance(GLFWwindow* window_handle);
        virtual ~render_instance();

        // no copies
        render_instance(const render_instance&) = delete;
        render_instance& operator=(const render_instance&) = delete;

        // allow move
        render_instance(render_instance&&) = default;
        render_instance& operator=(render_instance&&) = default;

        /* Functions */
        void create_pipeline();
        void window_resized();

        swapchain& get_swapchain();

        const surface&  get_surface() const;
        const pipeline& get_pipeline() const;
        const device&   get_device() const;
    };

}  // namespace potato::graphics

#endif  // !POTATO_RENDER_HPP
