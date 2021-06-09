#include "render.hpp"

#include <glm/glm.hpp>
#include <graphics/vkinclude/vulkan.hpp>
#include <tuple>

namespace testpotato {

    potato::render::pipeline_info render::create_pipeline_info() {

        auto pipeline_create_info {
            potato::render::pipeline::default_pipeline_info()
        };

        auto& info { pipeline_create_info.info };

        info.binding_descriptions =
          std::move(potato::vertex::binding_descriptions());

        info.attribute_descriptions =
          std::move(potato::vertex::attribute_descriptions());

        info.vertex_shader   = "shader.vert.spv";
        info.fragment_shader = "shader.frag.spv";
        info.subpass_count   = 0;

        return pipeline_create_info;
    }

    vk::UniquePipelineLayout
    render::create_pipeline_layout(const vk::Device& device) {
        constexpr auto shader_and_frag { vk::ShaderStageFlagBits::eVertex
                                         | vk::ShaderStageFlagBits::eFragment };

        vk::PushConstantRange push_const_ranges {
            .stageFlags = shader_and_frag,
            .offset     = 0,
            .size       = sizeof(potato::PushConstantData),
        };

        vk::PipelineLayoutCreateInfo pipeline_layout_ci {};

        return device.createPipelineLayoutUnique(pipeline_layout_ci);
    }

    vk::RenderPass render::create_renderpass(const vk::Device& device,
                                             vk::Format        color_format,
                                             vk::Format        depth_format) {

        // clang-format off
        vk::AttachmentDescription color_attachment = {
            .format         = color_format,
            .samples        = vk::SampleCountFlagBits::e1,      // VK_SAMPLE_COUNT_1_BIT,
            .loadOp         = vk::AttachmentLoadOp::eClear,     // VK_ATTACHMENT_LOAD_OP_CLEAR,
            .storeOp        = vk::AttachmentStoreOp::eStore,    // VK_ATTACHMENT_STORE_OP_STORE,
            .stencilLoadOp  = vk::AttachmentLoadOp::eDontCare,  // VK_ATTACHMENT_LOAD_OP_DONT_CARE,
            .stencilStoreOp = vk::AttachmentStoreOp::eDontCare, // VK_ATTACHMENT_STORE_OP_DONT_CARE,
            .initialLayout  = vk::ImageLayout::eUndefined,      // VK_IMAGE_LAYOUT_UNDEFINED,
            .finalLayout    = vk::ImageLayout::ePresentSrcKHR,  // VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
        };

        vk::AttachmentReference color_attch_ref = {
            .attachment     = 0,
            .layout         = vk::ImageLayout::eColorAttachmentOptimal // VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
        };

        vk::AttachmentDescription depth_attachment {
            .format         = depth_format,
            .samples        = vk::SampleCountFlagBits::e1,      // VK_SAMPLE_COUNT_1_BIT;
            .loadOp         = vk::AttachmentLoadOp::eClear,     // VK_ATTACHMENT_LOAD_OP_CLEAR;
            .storeOp        = vk::AttachmentStoreOp::eDontCare, // VK_ATTACHMENT_STORE_OP_DONT_CARE;
            .stencilLoadOp  = vk::AttachmentLoadOp::eDontCare,  // VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            .stencilStoreOp = vk::AttachmentStoreOp::eDontCare, // VK_ATTACHMENT_STORE_OP_DONT_CARE;
            .initialLayout  = vk::ImageLayout::eUndefined,      // VK_IMAGE_LAYOUT_UNDEFINED;
            .finalLayout    = vk::ImageLayout::eDepthStencilAttachmentOptimal,  // VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        };

        vk::AttachmentReference depth_attch_ref {
            .attachment     = 1,
            .layout         = vk::ImageLayout::eDepthStencilAttachmentOptimal // VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
        };

        vk::SubpassDescription subpass_desc = {
            .pipelineBindPoint       = vk::PipelineBindPoint::eGraphics,
            .colorAttachmentCount    = 1,
            .pColorAttachments       = &color_attch_ref,
            .pDepthStencilAttachment = &depth_attch_ref,
        };

        // VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
        constexpr auto color_depth_write { vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentWrite };

        // { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT| VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT };
        constexpr auto src_mask { vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests };

        // { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT };
        constexpr auto dst_mask { vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests };

        vk::SubpassDependency subpass_dependency = {
            .srcSubpass    = VK_SUBPASS_EXTERNAL,
            .dstSubpass    = 0,
            .srcStageMask  = src_mask,
            .dstStageMask  = dst_mask,
            .srcAccessMask = {},
            .dstAccessMask = color_depth_write,
        };

        std::array<vk::AttachmentDescription, 2> attachments {
            color_attachment,
            depth_attachment
        };

        vk::RenderPassCreateInfo renderpass_ci = {
            .attachmentCount = static_cast<uint32_t>(attachments.size()),
            .pAttachments    = attachments.data(),
            .subpassCount    = 1,
            .pSubpasses      = &subpass_desc,
            .dependencyCount = 1,
            .pDependencies   = &subpass_dependency,
        };
        // clang-format on

        return device.createRenderPass(renderpass_ci);
    }

    void render::render_objects(const std::vector<potato::model>& objects) {

        auto command_buffer { get_surface().begin_frame(get_renderpass(),
                                                        get_pipeline()) };

        for ( auto& obj : objects ) {
            // obj.transform2d.rotation =
            //   glm::mod(obj.transform2d.rotation + 0.01f, glm::two_pi<float>());

            // PushConstantData push {};
            // push.offset    = obj.transform2d.translation;
            // push.color     = obj.color;
            // push.transform = obj.transform2d.mat2();

            // vkCmdPushConstants(commandBuffer,
            //                    pipelineLayout,
            //                    VK_SHADER_STAGE_VERTEX_BIT
            //                      | VK_SHADER_STAGE_FRAGMENT_BIT,
            //                    0,
            //                    sizeof(SimplePushConstantData),
            //                    &push);
            obj.bind(command_buffer);
            obj.draw(command_buffer);
        }

        get_surface().end_frame();
    }

}  // namespace testpotato
