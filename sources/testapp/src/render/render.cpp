module testapp : render;

import "glm.imp";

import : primitive;
import : camera;

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

        vk::PipelineLayoutCreateInfo pipeline_layout_ci {
            .pushConstantRangeCount = 1,
            .pPushConstantRanges    = &push_const_ranges,
        };

        auto pipeline_layout { device.createPipelineLayoutUnique(
          pipeline_layout_ci) };

        m_pipeline = std::move(pipeline(device,
                                        pipeline_create_info,
                                        std::move(pipeline_layout),
                                        renderpass));
    }

    void
    render_system::render_objects(const vk::CommandBuffer&           cmd_buffer,
                                  const std::vector<testapp::model>& objects,
                                  const camera&                      cam) {

        static push_constants push {};

        static constexpr auto shader_and_frag {
            vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment
        };

        auto projectionView = cam.getProjection() * cam.getView();

        m_pipeline.bind(cmd_buffer);

        for ( auto& obj : objects ) {

            push.transform = projectionView * obj.transform.mat4();

            cmd_buffer.pushConstants(m_pipeline.get_layout(),
                                     shader_and_frag,
                                     0,
                                     sizeof(push_constants),
                                     &push);

            obj.bind(cmd_buffer);
            obj.draw(cmd_buffer);
        }
    }

}  // namespace testapp
