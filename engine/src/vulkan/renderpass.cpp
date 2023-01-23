#include "vulkan/renderpass.h"
#include "rhi/context.h"
#include "vulkan/context.h"
#include "vulkan/device.h"
#include "vulkan/image.h"
#include "vulkan/resource/image.h"
#include "vulkan/resource/renderpassInfo.h"
#include <functional>
#include <vector>

namespace vk
{
Renderpass::Renderpass()
{
}

Result Renderpass::init(Context* context, rhi::RenderPassInfo& renderpassInfo, size_t hash)
{
    std::vector<VkAttachmentDescription> AttachmentDescriptions;

    for (auto& colorAttachment : renderpassInfo.ColorAttachmentDescriptions)
    {
        VkAttachmentDescription attachmentDescription = {};
        Image* image = reinterpret_cast<Image*>(colorAttachment.image);
        attachmentDescription.format = image->getFormat();
        attachmentDescription.samples = static_cast<VkSampleCountFlagBits>(colorAttachment.samples);
        attachmentDescription.loadOp = convertAttachmentLoadOp(colorAttachment.loadOp);
        attachmentDescription.storeOp = convertAttachmentStoreOp(colorAttachment.storeOp);
        attachmentDescription.initialLayout = convertImageLayout(colorAttachment.initialLayout);
        attachmentDescription.finalLayout = convertImageLayout(colorAttachment.finalLayout);
        AttachmentDescriptions.push_back(attachmentDescription);
    }

    size_t resolveAttachmentIndex = AttachmentDescriptions.size();
    if (renderpassInfo.ResolveAttachmentDescription.has_value())
    {
        auto& resolveAttachment = renderpassInfo.ResolveAttachmentDescription.value();

        VkAttachmentDescription attachmentDescription = {};
        Image* image = reinterpret_cast<Image*>(resolveAttachment.image);
        attachmentDescription.format = image->getFormat();
        attachmentDescription.samples = static_cast<VkSampleCountFlagBits>(resolveAttachment.samples);
        attachmentDescription.loadOp = convertAttachmentLoadOp(resolveAttachment.loadOp);
        attachmentDescription.storeOp = convertAttachmentStoreOp(resolveAttachment.storeOp);
        attachmentDescription.initialLayout = convertImageLayout(resolveAttachment.initialLayout);
        attachmentDescription.finalLayout = convertImageLayout(resolveAttachment.finalLayout);
        AttachmentDescriptions.push_back(attachmentDescription);
    }

    size_t depthStencilAttachmentIndex = AttachmentDescriptions.size();
    if (renderpassInfo.DepthStencilAttachmentDescription.has_value())
    {
        auto& depthStencilAttachment = renderpassInfo.DepthStencilAttachmentDescription.value();

        VkAttachmentDescription attachmentDescription = {};
        Image* image = reinterpret_cast<Image*>(depthStencilAttachment.image);
        attachmentDescription.format = image->getFormat();
        attachmentDescription.samples = static_cast<VkSampleCountFlagBits>(depthStencilAttachment.samples);
        attachmentDescription.loadOp = convertAttachmentLoadOp(depthStencilAttachment.loadOp);
        attachmentDescription.storeOp = convertAttachmentStoreOp(depthStencilAttachment.storeOp);
        attachmentDescription.initialLayout = convertImageLayout(depthStencilAttachment.initialLayout);
        attachmentDescription.finalLayout = convertImageLayout(depthStencilAttachment.finalLayout);
        AttachmentDescriptions.push_back(attachmentDescription);
    }

    // TODO: only 1 subpass for now
    // TODO: No input attachment for now
    std::vector<VkSubpassDescription> subpassDescriptions;
    std::vector<std::vector<VkAttachmentReference>> AttachmentReferences;
    for (auto& subpass : renderpassInfo.subpassDescriptions)
    {
        auto& AttachmentReference = AttachmentReferences.emplace_back();

        for (auto& colorReference : subpass.colorAttachmentReference)
        {
            AttachmentReference.push_back(
                {static_cast<uint32_t>(colorReference.id), convertImageLayout(colorReference.imageLayout)});
        }
        VkSubpassDescription subpassDescription = {};
        subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpassDescription.colorAttachmentCount = static_cast<uint32_t>(AttachmentReference.size());
        subpassDescription.pColorAttachments = AttachmentReference.data();

        if (subpass.resolveAttachmentReference.has_value())
        {
            auto& resolveAttachment = subpass.resolveAttachmentReference.value();
            AttachmentReference.push_back(
                {static_cast<uint32_t>(resolveAttachment.id), VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL});
            subpassDescription.pResolveAttachments = &AttachmentReference.back();
        }

        if (subpass.depthAttachmentReference.has_value())
        {
            auto& depthStencilAttachment = subpass.depthAttachmentReference.value();
            AttachmentReference.push_back(
                {static_cast<uint32_t>(depthStencilAttachment.id), VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL});
            subpassDescription.pDepthStencilAttachment = &AttachmentReference.back();
        }
        subpassDescriptions.push_back(subpassDescription);
    }
    // TODO: only 1 subpass for now
    std::vector<VkSubpassDependency> subpassDependency;
    {
    }

    VkRenderPassCreateInfo renderpassCreateInfo = {};
    renderpassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderpassCreateInfo.attachmentCount = static_cast<uint32_t>(AttachmentDescriptions.size());
    renderpassCreateInfo.pAttachments = AttachmentDescriptions.data();
    renderpassCreateInfo.subpassCount = static_cast<uint32_t>(subpassDescriptions.size());
    renderpassCreateInfo.pSubpasses = subpassDescriptions.data();
    renderpassCreateInfo.dependencyCount = static_cast<uint32_t>(subpassDependency.size());
    renderpassCreateInfo.pDependencies = subpassDependency.data();

    Device* device = context->getDevice();
    vk_try(create(device->getHandle(), renderpassCreateInfo));

    compatibleHash = hash;

    return Result::Continue;
}

void Renderpass::terminate(VkDevice device)
{
    if (valid())
    {
        vkDestroyRenderPass(device, mHandle, nullptr);
        mHandle = VK_NULL_HANDLE;
    }
}

VkResult Renderpass::create(VkDevice device, const VkRenderPassCreateInfo& createInfo)
{
    ASSERT(!valid());
    return vkCreateRenderPass(device, &createInfo, nullptr, &mHandle);
}
} // namespace vk