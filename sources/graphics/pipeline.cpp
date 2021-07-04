#include "pipeline.hpp"

#include "device/device.hpp"
#include "utils/utils.hpp"

#include <format>
#include <iostream>

namespace potato::graphics {
    using namespace potato::utils;

    // TODO: Make pipeline moveable to retain pointers
    pipeline::pipeline(const vk::Device&          device,
                       vkpipeline_info            pinf,
                       vk::UniquePipelineLayout&& pipeline_layout,
                       const vk::RenderPass&      renderpass)
      : m_pipeline_info { pinf }
      , m_pipeline_layout { std::move(pipeline_layout) } {

        auto vert_shader { create_shader(device, pinf.vertex_shader) };
        auto frag_shader { create_shader(device, pinf.fragment_shader) };

        std::vector<vk::PipelineShaderStageCreateInfo> shaders {
            {
              .stage  = vk::ShaderStageFlagBits::eVertex,
              .module = vert_shader,
              .pName  = "main",
            },
            {
              .stage  = vk::ShaderStageFlagBits::eFragment,
              .module = frag_shader,
              .pName  = "main",
            },
        };

        vk::PipelineVertexInputStateCreateInfo vertex_input_sci {
            .vertexBindingDescriptionCount = UINTSIZE(pinf.binding_descriptions),

            .pVertexBindingDescriptions = pinf.binding_descriptions.data(),

            .vertexAttributeDescriptionCount =
              UINTSIZE(pinf.attribute_descriptions),

            .pVertexAttributeDescriptions = pinf.attribute_descriptions.data(),
        };

        vk::PipelineDynamicStateCreateInfo dynamic_info_ci {
            .dynamicStateCount = UINTSIZE(pinf.ci_dynamic),
            .pDynamicStates    = pinf.ci_dynamic.data()
        };

        vk::GraphicsPipelineCreateInfo graphics_pipeline_ci {
            .pVertexInputState   = &vertex_input_sci,
            .pInputAssemblyState = &pinf.ci_input_assembly,
            .pTessellationState  = &pinf.ci_tessellation,
            .pViewportState      = &pinf.ci_viewport,
            .pRasterizationState = &pinf.ci_rasterization,
            .pMultisampleState   = &pinf.ci_multisample,
            .pDepthStencilState  = &pinf.ci_depth,
            .pColorBlendState    = &pinf.ci_colorblend,
            .pDynamicState       = &dynamic_info_ci,
            .layout              = *m_pipeline_layout,
            .renderPass          = renderpass,
            .subpass             = pinf.subpass_count
        };

        graphics_pipeline_ci.setPStages(shaders.data());
        graphics_pipeline_ci.setStageCount(UINTSIZE(shaders));

        auto create_result =
          device.createGraphicsPipelineUnique(nullptr, graphics_pipeline_ci);

        if ( create_result.result != vk::Result::eSuccess ) {
            throw std::runtime_error(
              std::format("Could not create graphics pipeline: {}\n",
                          vk::to_string(create_result.result)));
        }

        m_pipeline = std::move(create_result.value);

        device.destroyShaderModule(vert_shader);
        device.destroyShaderModule(frag_shader);
    }

    void pipeline::bind(const vk::CommandBuffer& cmd_buffer) const {
        cmd_buffer.bindPipeline(vk::PipelineBindPoint::eGraphics, *m_pipeline);
    }

    const vk::PipelineLayout& pipeline::get_layout() const {
        return *m_pipeline_layout;
    }

    vk::ShaderModule pipeline::create_shader(const vk::Device&  device,
                                             const std::string& fpath) {
        auto data { read_file(fpath) };
        return device.createShaderModule(vk::ShaderModuleCreateInfo {
          .codeSize = data.size(),
          .pCode    = reinterpret_cast<uint32_t*>(data.data()) });
    }

    vkpipeline_info pipeline::default_pipeline_info() {

        using ccfb = vk::ColorComponentFlagBits;

        std::vector<vk::DynamicState> dynamic_states {
            vk::DynamicState::eViewport,
            vk::DynamicState::eScissor,
        };

        vkpipeline_info pipelineinfo {
            .viewports = { {
                .x        = 0,
                .y        = 0,
                .width    = {},
                .height   = {},
                .minDepth = 0.0f,
                .maxDepth = 1.0f
            } },
            .scissors = { {
                .offset = {},
                .extent = {},
            } },
            // .ci_vertex_input  = {},
            .ci_input_assembly  = {
                .topology               = vk::PrimitiveTopology::eTriangleList,
                .primitiveRestartEnable = false
            },
            // .ci_tessellation  = {},
            .ci_viewport = {
                .viewportCount = UINTSIZE(pipelineinfo.viewports),
                .pViewports = pipelineinfo.viewports.data(),
                .scissorCount = UINTSIZE(pipelineinfo.scissors),
                .pScissors  = pipelineinfo.scissors.data(),
            },
            .ci_rasterization = {
                .depthClampEnable        = false,
                .rasterizerDiscardEnable = false,
                .polygonMode             = vk::PolygonMode::eFill,
                .cullMode                = vk::CullModeFlagBits::eNone,
                .frontFace               = vk::FrontFace::eClockwise,
                .depthBiasEnable         = false,
                // .depthBiasConstantFactor = {},
                // .depthBiasClamp = {},
                // .depthBiasSlopeFactor = {},
                .lineWidth = 1.0f,
            },
            .ci_multisample = {
                .rasterizationSamples  = vk::SampleCountFlagBits::e1,
                .sampleShadingEnable   = false,
                .minSampleShading      = 1.0f,
                .pSampleMask           = {},
                .alphaToCoverageEnable = false,
                .alphaToOneEnable      = false,
            },
            .ci_depth = {
                .depthTestEnable       = true,
                .depthWriteEnable      = true,
                .depthCompareOp        = vk::CompareOp::eLess,
                .depthBoundsTestEnable = false,
                .stencilTestEnable     = false,
                .front                 = {},
                .back                  = {},
                .minDepthBounds        = 0.0f,
                .maxDepthBounds        = 1.0f,
            },
            .colorblend_attachments = { {
                .blendEnable         = false,
                .srcColorBlendFactor = vk::BlendFactor::eOne,
                .dstColorBlendFactor = vk::BlendFactor::eZero,
                .colorBlendOp        = vk::BlendOp::eAdd,
                .srcAlphaBlendFactor = vk::BlendFactor::eOne,
                .dstAlphaBlendFactor = vk::BlendFactor::eZero,
                .alphaBlendOp        = vk::BlendOp::eAdd,
                .colorWriteMask      = ccfb::eA | ccfb::eR | ccfb::eG | ccfb::eB,
            } },
            .ci_colorblend = {
                .logicOpEnable = false,
                .logicOp = vk::LogicOp::eCopy,
                .attachmentCount = UINTSIZE(pipelineinfo.colorblend_attachments),
                .pAttachments = pipelineinfo.colorblend_attachments.data(),
                .blendConstants = {},
            },
            .ci_dynamic = dynamic_states,
            // .pipeline_layout = {},
            // .renderpass = {},
            .subpass_count = {},
        };

        return pipelineinfo;
    }

}  // namespace potato::graphics
