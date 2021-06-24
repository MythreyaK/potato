#include "pipeline.hpp"

#include "device/device.hpp"
#include "utils/utils.hpp"

#include <format>
#include <iostream>

namespace potato::render {
    using namespace potato::utils;

    // TODO: Make pipeline moveable to retain pointers
    pipeline::pipeline(std::shared_ptr<const device> device,
                       pipeline_info                 pinf,
                       vk::UniquePipelineLayout&&    pipeline_layout,
                       const vk::RenderPass&         renderpass)
      : logical_device(device)
      , vkpipeline_layout(std::move(pipeline_layout)) {

        auto& info = pinf.info;

        auto vert_shader { create_shader(info.vertex_shader) };
        auto frag_shader { create_shader(info.fragment_shader) };

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
            .vertexBindingDescriptionCount =
              static_cast<uint32_t>(info.binding_descriptions.size()),

            .pVertexBindingDescriptions = info.binding_descriptions.data(),

            .vertexAttributeDescriptionCount =
              static_cast<uint32_t>(info.attribute_descriptions.size()),

            .pVertexAttributeDescriptions = info.attribute_descriptions.data(),
        };

        vk::PipelineDynamicStateCreateInfo dynamic_info_ci {
            .dynamicStateCount = static_cast<uint32_t>(info.ci_dynamic.size()),
            .pDynamicStates    = info.ci_dynamic.data()
        };

        vk::GraphicsPipelineCreateInfo graphics_pipeline_ci {
            .pVertexInputState   = &vertex_input_sci,
            .pInputAssemblyState = &info.ci_input_assembly,
            .pTessellationState  = &info.ci_tessellation,
            .pViewportState      = &info.ci_viewport,
            .pRasterizationState = &info.ci_rasterization,
            .pMultisampleState   = &info.ci_multisample,
            .pDepthStencilState  = &info.ci_depth,
            .pColorBlendState    = &info.ci_colorblend,
            .pDynamicState       = &dynamic_info_ci,
            .layout              = *vkpipeline_layout,
            .renderPass          = renderpass,
            .subpass             = info.subpass_count
        };

        graphics_pipeline_ci.setPStages(shaders.data());
        graphics_pipeline_ci.setStageCount(
          static_cast<uint32_t>(shaders.size()));

        auto create_result =
          logical_device->logical->createGraphicsPipelineUnique(
            nullptr,
            graphics_pipeline_ci);

        if ( create_result.result != vk::Result::eSuccess ) {
            throw std::runtime_error(
              std::format("Could not create graphics pipeline: {}\n",
                          vk::to_string(create_result.result)));
        }

        vkpipeline = std::move(create_result.value);

        logical_device->logical->destroyShaderModule(vert_shader);
        logical_device->logical->destroyShaderModule(frag_shader);
    }

    void pipeline::bind(const vk::CommandBuffer& cmd_buffer) const {
        cmd_buffer.bindPipeline(vk::PipelineBindPoint::eGraphics, *vkpipeline);
    }

    vk::ShaderModule pipeline::create_shader(const std::string& fpath) {
        auto data { read_file(fpath) };
        return logical_device->logical->createShaderModule(
          vk::ShaderModuleCreateInfo {
            .codeSize = data.size(),
            .pCode    = reinterpret_cast<uint32_t*>(data.data()) });
    }

    pipeline_info pipeline::default_pipeline_info() {

        using ccfb = vk::ColorComponentFlagBits;

        std::vector<vk::DynamicState> dynamic_states {
            vk::DynamicState::eViewport,
            vk::DynamicState::eScissor,
        };

        _pif pipelineinfo {
            .viewport = {
                .x        = 0,
                .y        = 0,
                .width    = {},
                .height   = {},
                .minDepth = 0.0f,
                .maxDepth = 1.0f
            },
            .scissor = {
                .offset = {},
                .extent = {},
            },
            // .ci_vertex_input  = {},
            .ci_input_assembly  = {
                .topology               = vk::PrimitiveTopology::eTriangleList,
                .primitiveRestartEnable = false
            },
            // .ci_tessellation  = {},
            .ci_viewport = {
                .viewportCount = 1,
                .pViewports = &pipelineinfo.viewport,
                .scissorCount = 1,
                .pScissors  = &pipelineinfo.scissor,
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
            .colorblend_attachment = {
                .blendEnable         = false,
                .srcColorBlendFactor = vk::BlendFactor::eOne,
                .dstColorBlendFactor = vk::BlendFactor::eZero,
                .colorBlendOp        = vk::BlendOp::eAdd,
                .srcAlphaBlendFactor = vk::BlendFactor::eOne,
                .dstAlphaBlendFactor = vk::BlendFactor::eZero,
                .alphaBlendOp        = vk::BlendOp::eAdd,
                .colorWriteMask      = ccfb::eA | ccfb::eR | ccfb::eG | ccfb::eB,
            },
            .ci_colorblend = {
                .logicOpEnable = false,
                .logicOp = vk::LogicOp::eCopy,
                .attachmentCount = 1,
                .pAttachments = &pipelineinfo.colorblend_attachment,
                .blendConstants = {},
            },
            .ci_dynamic = dynamic_states,
            // .pipeline_layout = {},
            // .renderpass = {},
            .subpass_count = {},
        };

        return { pipelineinfo };
    }

#pragma region PIPELINEINFO_STRUCT

    pipeline_info::pipeline_info(const pipeline_info& other) {
        this->info = other.info;
        update_pointers();
    }

    pipeline_info::pipeline_info(const _pif& other) {
        this->info = other;
        update_pointers();
    }

    pipeline_info& pipeline_info::operator=(const pipeline_info& other) {
        this->info = other.info;
        update_pointers();
        return *this;
    }

    void pipeline_info::update_pointers() {
        auto& pif = this->info;

        // update pointers
        pif.ci_colorblend.pAttachments = &pif.colorblend_attachment;

        pif.ci_viewport.pViewports = &pif.viewport;
        pif.ci_viewport.pScissors  = &pif.scissor;
    }

#pragma endregion PIPELINEINFO_STRUCT

}  // namespace potato::render
