#ifndef POTATO_RENDER_PIPELINE_HPP
#define POTATO_RENDER_PIPELINE_HPP

// #include "vkinclude/vulkan.hpp"

#include <memory>

namespace potato::graphics {
    class device;

    using vkvertex_bindings = std::vector<vk::VertexInputBindingDescription>;

    using vkvertex_attributes =
      std::vector<vk::VertexInputAttributeDescription>;

    using vkcolorblend_attachments =
      std::vector<vk::PipelineColorBlendAttachmentState>;

    struct vkpipeline_info {
        std::vector<vk::Viewport>                viewports {};
        std::vector<vk::Rect2D>                  scissors {};
        vk::PipelineCreateFlags                  flags {};
        std::string                              vertex_shader {};
        std::string                              fragment_shader {};
        vkvertex_bindings                        binding_descriptions {};
        vkvertex_attributes                      attribute_descriptions {};
        vk::PipelineInputAssemblyStateCreateInfo ci_input_assembly {};
        vk::PipelineTessellationStateCreateInfo  ci_tessellation {};
        vk::PipelineViewportStateCreateInfo      ci_viewport {};
        vk::PipelineRasterizationStateCreateInfo ci_rasterization {};
        vk::PipelineMultisampleStateCreateInfo   ci_multisample {};
        vk::PipelineDepthStencilStateCreateInfo  ci_depth {};
        vkcolorblend_attachments                 colorblend_attachments {};
        vk::PipelineColorBlendStateCreateInfo    ci_colorblend {};
        std::vector<vk::DynamicState>            ci_dynamic {};
        uint32_t                                 subpass_count {};
    };

    class pipeline {

      private:
        vkpipeline_info          m_pipeline_info {};
        vk::UniquePipelineLayout m_pipeline_layout {};
        vk::UniquePipeline       m_pipeline {};

        vk::ShaderModule create_shader(const vk::Device&, const std::string&);

      public:
        pipeline() = default;
        pipeline(const vk::Device&,
                 vkpipeline_info,
                 vk::UniquePipelineLayout&&,
                 const vk::RenderPass&);

        void bind(const vk::CommandBuffer&) const;

        static vkpipeline_info default_pipeline_info();

        // no copies
        pipeline(const pipeline&) = delete;
        pipeline& operator=(const pipeline&) = delete;

        // allow move
        pipeline(pipeline&&) = default;
        pipeline& operator=(pipeline&&) = default;
    };

}  // namespace potato::graphics

#endif
