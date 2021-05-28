#ifndef POTATO_RENDER_PIPELINE_HPP
#define POTATO_RENDER_PIPELINE_HPP

#include "vkinclude/vulkan.hpp"

namespace potato::render {

    struct _pif {
        vk::Viewport                             viewport {};
        vk::Rect2D                               scissor {};
        vk::PipelineCreateFlags                  flags {};
        vk::PipelineShaderStageCreateInfo        ci_shaders {};
        vk::PipelineVertexInputStateCreateInfo   ci_vertex_input {};
        vk::PipelineInputAssemblyStateCreateInfo ci_input_assembly {};
        vk::PipelineTessellationStateCreateInfo  ci_tessellation {};
        vk::PipelineViewportStateCreateInfo      ci_viewport {};
        vk::PipelineRasterizationStateCreateInfo ci_rasterization {};
        vk::PipelineMultisampleStateCreateInfo   ci_multisample {};
        vk::PipelineDepthStencilStateCreateInfo  ci_depth {};
        vk::PipelineColorBlendAttachmentState    colorblend_attachment {};
        vk::PipelineColorBlendStateCreateInfo    ci_colorblend {};
        vk::PipelineDynamicStateCreateInfo       ci_dynamic {};
        vk::PipelineLayout                       layout {};
        vk::RenderPass                           renderpass {};
        uint32_t                                 subpass {};
        vk::Pipeline                             old_pipeline {};
        int32_t                                  pipeline_inx {};
    };

    struct pipeline_info {
        // This exists to allow for updating the
        // pointers when _pif is copied, and to
        // also allow aggregate-initialization of _pif
        _pif info {};

        pipeline_info() = default;
        pipeline_info(_pif&&);
        pipeline_info(const _pif&);

        pipeline_info(const pipeline_info&);
        pipeline_info(pipeline_info&&) = default;

        pipeline_info& operator=(const pipeline_info&);
        pipeline_info& operator=(pipeline_info&&) = default;

      private:
        void update_pointers();
    };

    class device;

    // TOTO: Ughhh... might need to clean this up a little
    class pipeline {

      private:
        device&                        logical_device;
        pipeline_info                  pipelineinfo;
        vk::ShaderModule               vertex_shader;
        vk::ShaderModule               fragment_shader;
        vk::GraphicsPipelineCreateInfo gp_ci;
        vk::PipelineLayout             pipeline_layout;
        vk::RenderPass                 renderpass;
        vk::Pipeline                   vkpipeline;

        vk::ShaderModule create_shader(const std::string& fpath);

      public:
        pipeline(device&,
                 const std::string& vert,
                 const std::string& frag,
                 pipeline_info);
        ~pipeline();

        void build();
        void set_layout(const vk::PipelineLayoutCreateInfo&);
        void set_renderpass(const vk::RenderPassCreateInfo&);

        static pipeline_info default_pipeline_info(const vk::Extent2D&);

        // no copies
        pipeline(const pipeline&) = delete;
        pipeline& operator=(const pipeline&) = delete;
    };

}  // namespace potato::render

#endif
