#include "pipeline.hpp"

#include "device.hpp"
#include "utils.hpp"

#include <format>
#include <iostream>

namespace potato::render {

    pipeline::pipeline(device&            device,
                       const std::string& vert,
                       const std::string& frag,
                       pipeline_info      pinf)
      : logical_device(device)
      , pipelineinfo(pinf)
      , vertex_shader(create_shader(vert))
      , fragment_shader(create_shader(frag)) {

        auto& info = pipelineinfo.info;

        std::vector<vk::PipelineShaderStageCreateInfo> shaders {
            {
              .stage  = vk::ShaderStageFlagBits::eVertex,
              .module = vertex_shader,
              .pName  = "main",
            },
            {
              .stage  = vk::ShaderStageFlagBits::eFragment,
              .module = fragment_shader,
              .pName  = "main",
            },
        };

        info.ci_vertex_input = vk::PipelineVertexInputStateCreateInfo {
            .flags                           = {},
            .vertexBindingDescriptionCount   = {},
            .pVertexBindingDescriptions      = {},
            .vertexAttributeDescriptionCount = {},
            .pVertexAttributeDescriptions    = {},
        };

        vk::GraphicsPipelineCreateInfo graphics_pipeline_ci {
            .pVertexInputState   = &info.ci_vertex_input,
            .pInputAssemblyState = &info.ci_input_assembly,
            .pTessellationState  = &info.ci_tessellation,
            .pViewportState      = &info.ci_viewport,
            .pRasterizationState = &info.ci_rasterization,
            .pMultisampleState   = &info.ci_multisample,
            .pDepthStencilState  = &info.ci_depth,
            .pColorBlendState    = &info.ci_colorblend,
            .pDynamicState       = &info.ci_dynamic,
            .layout              = info.layout,
            .renderPass          = info.renderpass,
            .subpass             = info.subpass,
            .basePipelineHandle  = info.old_pipeline,
            .basePipelineIndex   = info.pipeline_inx,
        };

        vk::PipelineLayoutCreateInfo pipeline_layout_ci {};

        std::vector<vk::AttachmentDescription> attachments {
            {
              // color attachment
              .format         = logical_device.format(),
              .samples        = vk::SampleCountFlagBits::e1,
              .loadOp         = vk::AttachmentLoadOp::eClear,
              .storeOp        = vk::AttachmentStoreOp::eStore,
              .stencilLoadOp  = vk::AttachmentLoadOp::eDontCare,
              .stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
              .initialLayout  = vk::ImageLayout::eUndefined,
              .finalLayout    = vk::ImageLayout::ePresentSrcKHR,
            },
            // {
            //   // depth attachment. TODO: Pick proper format
            //   .format         = vk::Format::eD32Sfloat,
            //   .samples        = vk::SampleCountFlagBits::e1,
            //   .loadOp         = vk::AttachmentLoadOp::eClear,
            //   .storeOp        = vk::AttachmentStoreOp::eStore,
            //   .stencilLoadOp  = vk::AttachmentLoadOp::eDontCare,
            //   .stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
            //   .initialLayout  = vk::ImageLayout::eUndefined,
            //   .finalLayout    = vk::ImageLayout::eDepthStencilAttachmentOptimal,

            // }
        };

        std::vector<vk::AttachmentReference> attachment_refs {
            {
              // color attachment
              .attachment = 0,
              .layout     = vk::ImageLayout::eColorAttachmentOptimal,
            },
            // {
            //   // depth attachment
            //   .attachment = 1,
            //   .layout     = vk::ImageLayout::eDepthStencilAttachmentOptimal,
            // }
        };

        std::vector<vk::SubpassDescription> subpass_desc { {
          .pipelineBindPoint = vk::PipelineBindPoint::eGraphics,
          // .inputAttachmentCount = {},
          // .pInputAttachments = {},
          .colorAttachmentCount = 1,
          .pColorAttachments    = &attachment_refs[0],
          // .pResolveAttachments = {},
        //   .pDepthStencilAttachment = &attachment_refs[1],
          // .preserveAttachmentCount = {},
          // .pPreserveAttachments = {},
        } };

        std::vector<vk::SubpassDependency> subpass_deps { {
          .srcSubpass      = VK_SUBPASS_EXTERNAL,
          .dstSubpass      = {},
          .srcStageMask    = vk::PipelineStageFlagBits::eColorAttachmentOutput,
          .dstStageMask    = vk::PipelineStageFlagBits::eColorAttachmentOutput,
          .srcAccessMask   = vk::AccessFlagBits::eNoneKHR,
          .dstAccessMask   = vk::AccessFlagBits::eColorAttachmentWrite,
          .dependencyFlags = {},
        } };

        vk::RenderPassCreateInfo rp_ci {
            .flags           = {},
            .attachmentCount = static_cast<uint32_t>(attachments.size()),
            .pAttachments    = attachments.data(),
            .subpassCount    = static_cast<uint32_t>(subpass_desc.size()),
            .pSubpasses      = subpass_desc.data(),
            .dependencyCount = static_cast<uint32_t>(subpass_deps.size()),
            .pDependencies   = subpass_deps.data(),
        };

        graphics_pipeline_ci.setPStages(shaders.data());
        graphics_pipeline_ci.setStageCount(
          static_cast<uint32_t>(shaders.size()));

        pipeline_layout =
          logical_device.get().createPipelineLayout(pipeline_layout_ci);
        renderpass = logical_device.get().createRenderPass(rp_ci);

        graphics_pipeline_ci.setLayout(pipeline_layout);
        graphics_pipeline_ci.setRenderPass(renderpass);

        auto create_result =
          logical_device.get().createGraphicsPipeline(nullptr,
                                                      graphics_pipeline_ci);

        if ( create_result.result != vk::Result::eSuccess ) {
            throw std::runtime_error(
              std::format("Could not create graphics pipeline: {}\n",
                          vk::to_string(create_result.result)));
        }

        vkpipeline = std::move(create_result.value);

        // create framebuffers
        logical_device.create_framebuffers(renderpass);

        logical_device.get().destroyShaderModule(vertex_shader);
        logical_device.get().destroyShaderModule(fragment_shader);
    }

    void pipeline::set_layout(const vk::PipelineLayoutCreateInfo& ci) {
        gp_ci.setLayout(logical_device.get().createPipelineLayout(ci));
    }

    void pipeline::set_renderpass(const vk::RenderPassCreateInfo& ci) {
        gp_ci.setRenderPass(logical_device.get().createRenderPass(ci));
    }

    void pipeline::build() {}

    pipeline::~pipeline() {
        logical_device.get().destroyPipeline(vkpipeline);
        logical_device.get().destroyPipelineLayout(pipeline_layout);
        logical_device.get().destroyRenderPass(renderpass);
    }

    vk::ShaderModule pipeline::create_shader(const std::string& fpath) {
        auto data { read_file(fpath) };
        return logical_device.get().createShaderModule(
          vk::ShaderModuleCreateInfo {
            .codeSize = data.size(),
            .pCode    = reinterpret_cast<uint32_t*>(data.data()) });
    }

    pipeline_info pipeline::default_pipeline_info(const vk::Extent2D& extents) {

        using ccfb = vk::ColorComponentFlagBits;

        _pif pipelineinfo {
            .viewport = {
                .x        = 0,
                .y        = 0,
                .width    = extents.width / 1.0f,
                .height   = extents.height / 1.0f,
                .minDepth = 0.0f,
                .maxDepth = 1.0f
            },
            .scissor = {
                .offset = {},
                .extent = extents,
            },
            // .ci_shaders  = {},
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
            // .ci_dynamic = {},
            // .layout = {},
            // .renderpass = {},
            // .subpass = {},
        };

        return { pipelineinfo };
    }

#pragma region PIPELINEINFO_STRUCT
    // copy-assignment and copy-constructor for pipeline_info
    pipeline_info& pipeline_info::operator=(const pipeline_info& other) {
        // do a normal copy
        this->info = other.info;
        update_pointers();
        return *this;
    }

    pipeline_info::pipeline_info(const pipeline_info& other) {
        // do a normal copy
        this->info = other.info;
        update_pointers();
    }

    pipeline_info::pipeline_info(_pif&& other) {
        this->info = std::move(other);
        update_pointers();
    }
    pipeline_info::pipeline_info(const _pif& other) {
        this->info = other;
        update_pointers();
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
