#include "device/device.hpp"
#include "swapchain.hpp"
#include <core/utils.hpp>

namespace potato::graphics {

    void swapchain::create_renderpass() {
        using namespace potato::utils;

        // clang-format off
        vk::AttachmentDescription color_attachment = {
            .format         = color_format(),
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
            .format         = depth_format(),
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
            .attachmentCount = vksize(attachments),
            .pAttachments    = attachments.data(),
            .subpassCount    = 1,
            .pSubpasses      = &subpass_desc,
            .dependencyCount = 1,
            .pDependencies   = &subpass_dependency,
        };
        // clang-format on

        m_renderpass =
          std::move(m_device->logical->createRenderPass(renderpass_ci));
    }

    const vk::RenderPass& swapchain::get_renderpass() const {
        return m_renderpass;
    }

    void swapchain::destroy_renderpass() {
        m_device->logical->destroyRenderPass(m_renderpass);
    }

}  // namespace potato::graphics
