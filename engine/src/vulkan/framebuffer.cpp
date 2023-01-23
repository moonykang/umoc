#include "framebuffer.h"
#include "common/hash.h"
#include "context.h"
#include "device.h"
#include "image.h"
#include "renderpass.h"
#include "resource/renderpassInfo.h"
#include <vector>

namespace vk
{
Framebuffer::Framebuffer() : framebufferHash(0)
{
}

Result Framebuffer::init(Context* context, rhi::RenderPassInfo& renderpassInfo, Renderpass* renderpass, size_t hash)
{
    std::vector<VkImageView> views;
    VkExtent3D extent;

    for (auto& colorAttachment : renderpassInfo.ColorAttachmentDescriptions)
    {
        Image* image = reinterpret_cast<Image*>(colorAttachment.image);
        extent = image->getExtent();
        views.push_back(image->getView());
    }

    if (renderpassInfo.ResolveAttachmentDescription.has_value())
    {
        auto& resolveAttachment = renderpassInfo.ResolveAttachmentDescription.value();
        Image* image = reinterpret_cast<Image*>(resolveAttachment.image);
        views.push_back(image->getView());
    }

    if (renderpassInfo.DepthStencilAttachmentDescription.has_value())
    {
        auto& depthStencilAttachment = renderpassInfo.DepthStencilAttachmentDescription.value();
        Image* image = reinterpret_cast<Image*>(depthStencilAttachment.image);
        views.push_back(image->getView());
    }

    VkFramebufferCreateInfo frameBufferCreateInfo = {};
    frameBufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    frameBufferCreateInfo.pNext = NULL;
    frameBufferCreateInfo.renderPass = renderpass->getHandle();
    frameBufferCreateInfo.attachmentCount = static_cast<uint32_t>(views.size());
    frameBufferCreateInfo.pAttachments = views.data();
    frameBufferCreateInfo.width = extent.width;
    frameBufferCreateInfo.height = extent.height;
    frameBufferCreateInfo.layers = 1;

    vk_try(create(context->getDevice()->getHandle(), frameBufferCreateInfo));

    framebufferHash = hash;

    return Result::Continue;
}

void Framebuffer::terminate(VkDevice device)
{
    if (valid())
    {
        vkDestroyFramebuffer(device, mHandle, nullptr);
        mHandle = VK_NULL_HANDLE;
    }
}

VkResult Framebuffer::create(VkDevice device, const VkFramebufferCreateInfo& createInfo)
{
    ASSERT(!valid());
    return vkCreateFramebuffer(device, &createInfo, nullptr, &mHandle);
}

bool Framebuffer::match(size_t hash)
{
    ASSERT(valid());
    return hash == framebufferHash;
}
} // namespace vk