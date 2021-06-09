#ifndef TESTAPP_RENDER_HPP
#define TESTAPP_RENDER_HPP

#include <graphics/render.hpp>
#include <graphics/vkinclude/vulkan.hpp>

#include "primitive.hpp"

namespace testpotato {

    namespace potato_render = potato::render;
    class render final : public potato::render::render_instance {

      public:
        render(GLFWwindow* window_handle) : render_instance(window_handle) {
            create_pipeline();
        }

        virtual potato_render::pipeline_info create_pipeline_info() override;

        virtual vk::UniquePipelineLayout
        create_pipeline_layout(const vk::Device&) override;

        virtual vk::RenderPass
        create_renderpass(const vk::Device&,
                          vk::Format color_format,
                          vk::Format depth_format) override;

        void render_objects(const std::vector<potato::model>& objects);
    };

}  // namespace testpotato

#endif
