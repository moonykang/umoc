#include "vulkan/rendertarget.h"
#include "commandBuffer.h"
#include "common/hash.h"
#include "context.h"
#include "queue.h"
#include "rhi/defines.h"
#include "rhi/rendertarget.h"
#include "vulkan/framebuffer.h"
#include "vulkan/image.h"
#include "vulkan/renderpass.h"
#include "vulkan/resource/renderpassInfo.h"

namespace vk
{

Result Context::beginRenderpass(rhi::RenderPassInfo& renderpassInfo)
{
    ASSERT(renderTargetManager);

    try(renderTargetManager->begin(this, renderpassInfo));

    return Result::Continue;
}

Result Context::endRenderpass()
{
    getActiveCommandBuffer()->endRenderPass();

    return Result::Continue;
}

size_t Context::getCurrentRenderpassHash()
{
    return renderTargetManager->getCurrentRenderPassHash();
}

Renderpass* Context::getCurrentRenderpass()
{
    return renderTargetManager->getCurrentRenderpass();
}

Result Context::viewport(rhi::Extent2D extent)
{
    VkViewport viewport = {};
    viewport.x = 0;
    viewport.y = 0;
    viewport.width = extent.width;
    viewport.height = extent.height;
    viewport.minDepth = 0.f;
    viewport.maxDepth = 1.f;

    getActiveCommandBuffer()->setViewport(viewport);

    return Result::Continue;
}

RenderTargetManager::RenderTargetManager() : currentRenderPassHash(0)
{
}

void RenderTargetManager::terminate(VkDevice device)
{
    for (auto& framebuffers : framebufferMap)
    {
        for (auto& framebuffer : framebuffers.second)
        {
            TERMINATE(framebuffer, device);
        }
    }
    framebufferMap.clear();

    for (auto& renderpass : renderpassMap)
    {
        TERMINATE(renderpass.second, device);
    }
    renderpassMap.clear();
}

Result RenderTargetManager::begin(Context* context, rhi::RenderPassInfo& renderpassInfo)
{
    size_t renderPassCompatibleHash = generateRenderpassCompatibleHash(renderpassInfo);
    size_t renderPassHash = generateRenderpassHash(renderpassInfo, renderPassCompatibleHash);
    size_t framebufferHash = generateFramebufferHash(renderpassInfo);

    // get renderpass
    Renderpass* renderpass = nullptr;
    {
        const std::lock_guard<std::mutex> lock(hashMutex);

        if (auto search = renderpassMap.find(renderPassHash); search != renderpassMap.end())
        {
            renderpass = search->second;
        }
        else
        {
            renderpass = new Renderpass();
            renderpass->init(context, renderpassInfo, renderPassCompatibleHash);
            renderpassMap.insert({renderPassHash, renderpass});
        }
    }

    ASSERT(renderpass && renderpass->valid());

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

            if (!framebuffer)
            {
                framebuffer = new Framebuffer();
                framebuffer->init(context, renderpassInfo, renderpass, framebufferHash);
                framebufferList.push_back(framebuffer);
            }
        }
        else
        {
            auto& framebufferList = framebufferMap.insert({renderPassHash, {}}).first->second;
            framebuffer = new Framebuffer();
            framebuffer->init(context, renderpassInfo, renderpass, framebufferHash);
            framebufferList.push_back(framebuffer);
        }
    }
    ASSERT(framebuffer && framebuffer->valid());

    CommandBuffer* commandBuffer = context->getActiveCommandBuffer();
    // Image layout

    std::vector<VkClearValue> clearValues;
    for (auto& colorAttachment : renderpassInfo.ColorAttachmentDescriptions)
    {
        Image* image = reinterpret_cast<Image*>(colorAttachment.image);
        commandBuffer->addTransition(image->updateImageLayoutAndBarrier(rhi::ImageLayout::ColorAttachment));

        VkClearValue clearValue = {};
        clearValue.color = {0.0f, 0.0f, 0.0f, 1.0f};
        clearValues.push_back(clearValue);
    }

    if (renderpassInfo.ResolveAttachmentDescription.has_value())
    {
        Image* image = reinterpret_cast<Image*>(renderpassInfo.ResolveAttachmentDescription.value().image);
        commandBuffer->addTransition(image->updateImageLayoutAndBarrier(rhi::ImageLayout::ColorAttachment));

        VkClearValue clearValue = {};
        clearValue.color = {0.0f, 0.0f, 0.0f, 1.0f};
        clearValues.push_back(clearValue);
    }

    if (renderpassInfo.DepthStencilAttachmentDescription.has_value())
    {
        Image* image = reinterpret_cast<Image*>(renderpassInfo.DepthStencilAttachmentDescription.value().image);
        commandBuffer->addTransition(image->updateImageLayoutAndBarrier(rhi::ImageLayout::DepthStencilAttachment));

        VkClearValue clearValue = {};
        clearValue.depthStencil = {1.f, 0};
        clearValues.push_back(clearValue);
    }
    commandBuffer->flushTransitions();

    rhi::Extent3D frameBufferExtent = framebuffer->getExtent();

    VkRenderPassBeginInfo renderpassBeginInfo = {};
    renderpassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderpassBeginInfo.pNext = nullptr;
    renderpassBeginInfo.renderPass = renderpass->getHandle();
    renderpassBeginInfo.framebuffer = framebuffer->getHandle();
    renderpassBeginInfo.renderArea = {0, 0, frameBufferExtent.width, frameBufferExtent.height};
    renderpassBeginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderpassBeginInfo.pClearValues = clearValues.data();

    try(context->submitUploadCommandBuffer());
    commandBuffer->beginRenderPass(renderpassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

    VkViewport viewport = {};
    viewport.x = static_cast<float>(renderpassBeginInfo.renderArea.offset.x);
    viewport.y = static_cast<float>(renderpassBeginInfo.renderArea.offset.y);
    viewport.width = static_cast<float>(renderpassBeginInfo.renderArea.extent.width);
    viewport.height = static_cast<float>(renderpassBeginInfo.renderArea.extent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    commandBuffer->setViewport(viewport);
    commandBuffer->setScissor(renderpassBeginInfo.renderArea);

    currentRenderPassHash = renderPassHash;

    return Result::Continue;
}

struct RenderpassHashStruct
{
    RenderpassHashStruct()
    {
        memset(this, 0, sizeof(RenderpassHashStruct));
    }

    size_t compatibleHash;
    VkAttachmentLoadOp loadOps[rhi::MaxSimultaneousRenderTargets];
    VkAttachmentStoreOp storeOps[rhi::MaxSimultaneousRenderTargets];
};

size_t RenderTargetManager::generateRenderpassHash(rhi::RenderPassInfo& renderpassInfo, size_t compatibleHash)
{
    RenderpassHashStruct renderpassHashStruct;
    renderpassHashStruct.compatibleHash = compatibleHash;

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

    return util::computeGenericHash(&renderpassHashStruct, sizeof(RenderpassHashStruct));
}

struct RenderpassCompatibleHashStruct
{
    RenderpassCompatibleHashStruct()
    {
        memset(this, 0, sizeof(RenderpassCompatibleHashStruct));
    }

    uint8_t numAttachments;
    uint8_t numSamples;

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

    return util::computeGenericHash(&renderpassCompatibleHashStruct, sizeof(RenderpassCompatibleHashStruct));
}

struct FramebufferHashStruct
{
    FramebufferHashStruct()
    {
        memset(this, 0, sizeof(FramebufferHashStruct));
    }

    uint8_t numAttachments;
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
    framebufferHashStruct.numAttachments = attachmentIndex;

    return util::computeGenericHash(&framebufferHashStruct, sizeof(FramebufferHashStruct));
}
} // namespace vk
