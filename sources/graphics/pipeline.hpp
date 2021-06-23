#ifndef POTATO_RENDER_PIPELINE_HPP
#define POTATO_RENDER_PIPELINE_HPP

// #include "vkinclude/vulkan.hpp"

#include <memory>

namespace potato::render {
    using vertex_bindings   = std::vector<vk::VertexInputBindingDescription>;
    using vertex_attributes = std::vector<vk::VertexInputAttributeDescription>;

    struct _pif {
        vk::Viewport                             viewport {};
        vk::Rect2D                               scissor {};
        vk::PipelineCreateFlags                  flags {};
        std::string                              vertex_shader {};
        std::string                              fragment_shader {};
        vertex_bindings                          binding_descriptions {};
        vertex_attributes                        attribute_descriptions {};
        vk::PipelineInputAssemblyStateCreateInfo ci_input_assembly {};
        vk::PipelineTessellationStateCreateInfo  ci_tessellation {};
        vk::PipelineViewportStateCreateInfo      ci_viewport {};
        vk::PipelineRasterizationStateCreateInfo ci_rasterization {};
        vk::PipelineMultisampleStateCreateInfo   ci_multisample {};
        vk::PipelineDepthStencilStateCreateInfo  ci_depth {};
        vk::PipelineColorBlendAttachmentState    colorblend_attachment {};
        vk::PipelineColorBlendStateCreateInfo    ci_colorblend {};
        vk::PipelineDynamicStateCreateInfo       ci_dynamic {};
        uint32_t                                 subpass_count {};
    };

    struct pipeline_info {
        // This exists to allow for updating the
        // pointers when _pif is copied, and to
        // also allow aggregate-initialization of _pif
        _pif info {};

        pipeline_info()                = default;
        ~pipeline_info()               = default;
        pipeline_info(pipeline_info&&) = default;

        pipeline_info(const _pif&);

        pipeline_info(const pipeline_info&);
        pipeline_info& operator=(const pipeline_info&);

        pipeline_info(_pif&&)  = delete;
        pipeline_info& operator=(pipeline_info&&) = delete;

      private:
        void update_pointers();
    };

    class device;

    class pipeline {

      private:
        std::shared_ptr<const device> logical_device {};
        vk::UniquePipelineLayout      vkpipeline_layout {};
        vk::UniquePipeline            vkpipeline {};

        vk::ShaderModule create_shader(const std::string& fpath);

      public:
        pipeline() = default;
        pipeline(std::shared_ptr<const device>,
                 pipeline_info,
                 vk::UniquePipelineLayout&&,
                 const vk::RenderPass&);

        void bind(const vk::CommandBuffer&) const;

        static pipeline_info default_pipeline_info();

        // no copies
        pipeline(const pipeline&) = delete;
        pipeline& operator=(const pipeline&) = delete;

        // allow move
        pipeline(pipeline&&) = default;
        pipeline& operator=(pipeline&&) = default;
    };

}  // namespace potato::render

#endif
