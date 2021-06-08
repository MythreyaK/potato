#ifndef POTATO_RENDER_HPP
#define POTATO_RENDER_HPP

// clang-format off
#include "vkinclude/vulkan.hpp"
// clang-format on
#include "context.hpp"
#include "device.hpp"
#include "pipeline.hpp"
#include "surface.hpp"

#include <vector>

namespace potato::render {

    class render_instance {
      private:
        GLFWwindow*             window_handle;
        context                 potato_context;
        std::shared_ptr<device> potato_device;
        surface                 potato_surface;
        vk::RenderPass          renderpass {};
        pipeline                potato_pipeline {};

        void destroy_objects();
        void recreate_objects();

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
        void            create_pipeline();
        void            window_resized();
        uint32_t        swapimage_count() const;
        const pipeline& get_pipeline() const;

        // Virtual functions
        virtual pipeline_info create_pipeline_info() = 0;

        virtual vk::UniquePipelineLayout
        create_pipeline_layout(const vk::Device&) = 0;

        virtual vk::RenderPass create_renderpass(const vk::Device&,
                                                 vk::Format color_format,
                                                 vk::Format depth_format) = 0;
    };

}  // namespace potato::render

#endif  // !POTATO_RENDER_HPP
