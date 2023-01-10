#include "vulkan/rendertarget.h"
#include "common/hash.h"
#include "vulkan/framebuffer.h"
#include "vulkan/image.h"
#include "vulkan/renderpass.h"
#include "vulkan/resource/renderpassInfo.h"

namespace vk
{

Result RenderTargetManager::begin(Context* context, rhi::RenderPassInfo& renderpassInfo)
{
    size_t renderPassHash = generateRenderpassHash(renderpassInfo);
    size_t renderPassCompatibleHash = generateRenderpassCompatibleHash(renderpassInfo);
    size_t framebufferHash = generateFramebufferHash(renderpassInfo);

    // get renderpass
    Renderpass* renderpass = nullptr;
    {
        const std::lock_guard<std::mutex> lock(hashMutex);

        if (auto search = renderpassMap.find(renderPassHash); search != renderpassMap.end())
        {
            renderpass = search->second;
        }

        if (!renderpass)
        {
            renderpass = new Renderpass();
            renderpass->init(context, renderpassInfo, renderPassCompatibleHash);
            renderpassMap.insert({renderPassHash, renderpass});
        }
    }
    ASSERT(renderpass->valid());

    // get framebuffer
    Framebuffer* framebuffer = nullptr;
    {
        const std::lock_guard<std::mutex> lock(hashMutex);
        if (auto search = framebufferMap.find(renderPassHash); search != framebufferMap.end())
        {
            auto& framebufferList = search->second;
            for (auto& candidFramebuffer : framebufferList)
            {
                if (candidFramebuffer->match(framebufferHash))
                {
                    framebuffer = candidFramebuffer;
                }
            }
        }

        if (!framebuffer)
        {
            framebuffer = new Framebuffer();
            framebuffer->init(context, renderpassInfo, renderpass, framebufferHash);
        }
    }
    ASSERT(framebuffer->valid());

    return Result::Continue;
}

struct RenderpassHashStruct
{
    RenderpassHashStruct()
    {
        memset(this, 0, sizeof(RenderpassHashStruct));
    }

    VkAttachmentLoadOp loadOps[rhi::MaxSimultaneousRenderTargets];
    VkAttachmentStoreOp storeOps[rhi::MaxSimultaneousRenderTargets];
};

size_t RenderTargetManager::generateRenderpassHash(rhi::RenderPassInfo& renderpassInfo)
{
    RenderpassHashStruct renderpassHashStruct;

    uint32_t attachmentIndex = 0;
    for (auto& colorAttachment : renderpassInfo.ColorAttachmentDescriptions)
    {
        renderpassHashStruct.loadOps[attachmentIndex] = convertAttachmentLoadOp(colorAttachment.loadOp);
        renderpassHashStruct.storeOps[attachmentIndex] = convertAttachmentStoreOp(colorAttachment.storeOp);
        attachmentIndex++;
    }

    if (renderpassInfo.ResolveAttachmentDescription.has_value())
    {
        auto& resolveAttachment = renderpassInfo.ResolveAttachmentDescription.value();
        renderpassHashStruct.loadOps[attachmentIndex] = convertAttachmentLoadOp(resolveAttachment.loadOp);
        renderpassHashStruct.storeOps[attachmentIndex] = convertAttachmentStoreOp(resolveAttachment.storeOp);
        attachmentIndex++;
    }

    if (renderpassInfo.DepthStencilAttachmentDescription.has_value())
    {
        auto& depthStencilAttachment = renderpassInfo.DepthStencilAttachmentDescription.value();
        renderpassHashStruct.loadOps[attachmentIndex] = convertAttachmentLoadOp(depthStencilAttachment.loadOp);
        renderpassHashStruct.storeOps[attachmentIndex] = convertAttachmentStoreOp(depthStencilAttachment.storeOp);
        attachmentIndex++;
    }

    return util::ComputeGenericHash(&renderpassHashStruct, sizeof(RenderpassHashStruct));
}

struct RenderpassCompatibleHashStruct
{
    RenderpassCompatibleHashStruct()
    {
        memset(this, 0, sizeof(RenderpassCompatibleHashStruct));
    }

    uint8 numAttachments;
    uint8 numSamples;

    VkFormat formats[rhi::MaxSimultaneousRenderTargets];
};

size_t RenderTargetManager::generateRenderpassCompatibleHash(rhi::RenderPassInfo& renderpassInfo)
{
    RenderpassCompatibleHashStruct renderpassCompatibleHashStruct;
    uint32_t attachmentIndex = 0;

    for (auto& colorAttachment : renderpassInfo.ColorAttachmentDescriptions)
    {
        Image* image = reinterpret_cast<Image*>(colorAttachment.image);
        renderpassCompatibleHashStruct.formats[attachmentIndex] = image->getFormat();
        renderpassCompatibleHashStruct.numSamples = image->getSamples();
        attachmentIndex++;
    }

    if (renderpassInfo.ResolveAttachmentDescription.has_value())
    {
        auto& resolveAttachment = renderpassInfo.ResolveAttachmentDescription.value();
        Image* image = reinterpret_cast<Image*>(resolveAttachment.image);
        renderpassCompatibleHashStruct.formats[attachmentIndex] = image->getFormat();
        attachmentIndex++;
    }

    if (renderpassInfo.DepthStencilAttachmentDescription.has_value())
    {
        auto& depthStencilAttachment = renderpassInfo.DepthStencilAttachmentDescription.value();
        Image* image = reinterpret_cast<Image*>(depthStencilAttachment.image);
        renderpassCompatibleHashStruct.formats[attachmentIndex] = image->getFormat();
        attachmentIndex++;
    }

    renderpassCompatibleHashStruct.numAttachments = attachmentIndex + 1;

    return util::ComputeGenericHash(&renderpassCompatibleHashStruct, sizeof(RenderpassCompatibleHashStruct));
}

struct FramebufferHashStruct
{
    FramebufferHashStruct()
    {
        memset(this, 0, sizeof(FramebufferHashStruct));
    }

    VkImageView views[rhi::MaxSimultaneousRenderTargets];
};

size_t RenderTargetManager::generateFramebufferHash(rhi::RenderPassInfo& renderpassInfo)
{
    FramebufferHashStruct framebufferHashStruct;

    uint32_t attachmentIndex = 0;

    for (auto& colorAttachment : renderpassInfo.ColorAttachmentDescriptions)
    {
        Image* image = reinterpret_cast<Image*>(colorAttachment.image);
        framebufferHashStruct.views[attachmentIndex] = image->getView();
        attachmentIndex++;
    }

    if (renderpassInfo.ResolveAttachmentDescription.has_value())
    {
        auto& resolveAttachment = renderpassInfo.ResolveAttachmentDescription.value();
        Image* image = reinterpret_cast<Image*>(resolveAttachment.image);
        framebufferHashStruct.views[attachmentIndex] = image->getView();
        attachmentIndex++;
    }

    if (renderpassInfo.DepthStencilAttachmentDescription.has_value())
    {
        auto& depthStencilAttachment = renderpassInfo.DepthStencilAttachmentDescription.value();
        Image* image = reinterpret_cast<Image*>(depthStencilAttachment.image);
        framebufferHashStruct.views[attachmentIndex] = image->getView();
        attachmentIndex++;
    }

    return util::ComputeGenericHash(&framebufferHashStruct, sizeof(FramebufferHashStruct));
}
} // namespace vk