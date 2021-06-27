#include "render.hpp"

#include <glm/glm.hpp>
#include <graphics/pipeline.hpp>
#include <graphics/vkinclude/vulkan.hpp>
#include <tuple>

namespace testapp {

    render_system::render_system(const vk::Device&     dev,
                                 const vk::RenderPass& rp) {
        create_pipeline(dev, rp);
    }

    void render_system::create_pipeline(const vk::Device&     device,
                                        const vk::RenderPass& renderpass) {
        using namespace potato::graphics;

        auto pipeline_create_info {
            potato::graphics::pipeline::default_pipeline_info()
        };

        pipeline_create_info.binding_descriptions =
          std::move(vertex::binding_descriptions());

        pipeline_create_info.attribute_descriptions =
          std::move(vertex::attribute_descriptions());

        pipeline_create_info.vertex_shader   = "shader.vert.spv";
        pipeline_create_info.fragment_shader = "shader.frag.spv";
        pipeline_create_info.subpass_count   = 0;

        constexpr auto shader_and_frag { vk::ShaderStageFlagBits::eVertex
                                         | vk::ShaderStageFlagBits::eFragment };

        vk::PushConstantRange push_const_ranges {
            .stageFlags = shader_and_frag,
            .offset     = 0,
            .size       = sizeof(push_constants),
        };

        vk::PipelineLayoutCreateInfo pipeline_layout_ci {};

        auto pipeline_layout { device.createPipelineLayoutUnique(
          pipeline_layout_ci) };

        m_pipeline = std::move(pipeline(device,
                                        pipeline_create_info,
                                        std::move(pipeline_layout),
                                        renderpass));
    }

    void
    render_system::render_objects(const vk::CommandBuffer&           cmd_buffer,
                                  const std::vector<testapp::model>& objects) {

        m_pipeline.bind(cmd_buffer);

        for ( auto& obj : objects ) {
            obj.bind(cmd_buffer);
            obj.draw(cmd_buffer);
        }
    }

}  // namespace testapp
