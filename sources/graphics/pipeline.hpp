#ifndef POTATO_RENDER_PIPELINE_HPP
#define POTATO_RENDER_PIPELINE_HPP

#include "vkinclude/vulkan.hpp"

#include <memory>

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
        std::unique_ptr<vk::RenderPass>          renderpass {};
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

        pipeline_info(pipeline_info&&) = default;

        ~pipeline_info() = default;

        pipeline_info(const pipeline_info&) = delete;
        pipeline_info& operator=(const pipeline_info&) = delete;

        pipeline_info& operator=(pipeline_info&&) = default;

      private:
        void update_pointers();
    };

    class device;

    class pipeline {

      private:
        device&          logical_device;
        pipeline_info    pipelineinfo;
        vk::ShaderModule vertex_shader;
        vk::ShaderModule fragment_shader;
        vk::Pipeline     vkpipeline;

        vk::ShaderModule create_shader(const std::string& fpath);

      public:
        pipeline(device&,
                 const std::string& vert,
                 const std::string& frag,
                 pipeline_info);
        ~pipeline();

        void bind_commandbuffer(const vk::CommandBuffer&);

        const vk::RenderPass& get_renderpass() const;

        static pipeline_info default_pipeline_info(const vk::Extent2D&);

        // no copies
        pipeline(const pipeline&) = delete;
        pipeline& operator=(const pipeline&) = delete;
    };

}  // namespace potato::render

#endif
