#include "transition.h"

namespace vk
{
Transition::Transition()
    : mSrcStageMask(0), mDstStageMask(0), mMemoryBarrierSrcAccess(0), mMemoryBarrierDstAccess(0), memoryBarriers(),
      mBufferMemoryBarriers(), mImageMemoryBarriers()
{
}

Transition::Transition(rhi::ImageLayout oldLayout, rhi::ImageLayout newLayout, VkImage image,
                       VkImageSubresourceRange subresourceRange)
    : mSrcStageMask(0), mDstStageMask(0), mMemoryBarrierSrcAccess(0), mMemoryBarrierDstAccess(0), memoryBarriers(),
      mBufferMemoryBarriers(), mImageMemoryBarriers()
{
    ASSERT(image);

    if (newLayout == oldLayout)
    {
        const ImageMemoryBarrierData& layoutData = kImageMemoryBarrierData[oldLayout];
        // RAR is not a hazard and doesn't require a barrier, especially as the image layout hasn't
        // changed.  The following asserts that such a barrier is not attempted.
        ASSERT(layoutData.type == ResourceAccess::Write);
        // No layout change, only memory barrier is required

        mergeMemoryBarrier(GetImageLayoutSrcStageMask(layoutData), GetImageLayoutDstStageMask(layoutData),
                           layoutData.srcAccessMask, layoutData.dstAccessMask);
    }
    else
    {
        const ImageMemoryBarrierData& transitionFrom = kImageMemoryBarrierData[oldLayout];
        const ImageMemoryBarrierData& transitionTo = kImageMemoryBarrierData[newLayout];
        VkPipelineStageFlags srcStageMask = transitionFrom.srcStageMask;
        VkPipelineStageFlags dstStageMask = transitionTo.dstStageMask;

        VkImageMemoryBarrier imageMemoryBarrier = {};
        imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        imageMemoryBarrier.srcAccessMask = transitionFrom.srcAccessMask;
        imageMemoryBarrier.dstAccessMask = transitionTo.dstAccessMask;
        imageMemoryBarrier.oldLayout = transitionFrom.layout;
        imageMemoryBarrier.newLayout = transitionTo.layout;
        imageMemoryBarrier.image = image;

        // Transition the whole resource.
        imageMemoryBarrier.subresourceRange = subresourceRange;
        mergeImageBarrier(srcStageMask, dstStageMask, imageMemoryBarrier);
    }
}

Transition::Transition(VkBuffer buffer, size_t offset, size_t size, VkPipelineStageFlags srcPipelineStage,
                       VkPipelineStageFlags dstPipelineStage, VkAccessFlags srcAccess, VkAccessFlags dstAccess)
    : mSrcStageMask(), mDstStageMask(), mMemoryBarrierSrcAccess(), mMemoryBarrierDstAccess(), memoryBarriers(),
      mBufferMemoryBarriers(), mImageMemoryBarriers()
{
    VkBufferMemoryBarrier bufferMemoryBarrier{};
    bufferMemoryBarrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
    bufferMemoryBarrier.srcAccessMask = srcAccess;
    bufferMemoryBarrier.dstAccessMask = dstAccess;
    bufferMemoryBarrier.buffer = buffer;
    bufferMemoryBarrier.offset = offset;
    bufferMemoryBarrier.size = size;
    mergeBufferBarrier(srcPipelineStage, dstPipelineStage, bufferMemoryBarrier);
}

bool Transition::IsShaderReadOnlyLayout(const ImageMemoryBarrierData& imageLayout)
{
    return imageLayout.layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
}

VkPipelineStageFlags Transition::GetImageLayoutSrcStageMask(const ImageMemoryBarrierData& transition)
{
    return transition.srcStageMask;
}

VkPipelineStageFlags Transition::GetImageLayoutDstStageMask(const ImageMemoryBarrierData& transition)
{
    return transition.dstStageMask;
}

// merge two barriers into one
void Transition::merge(Transition* other)
{
    mSrcStageMask |= other->mSrcStageMask;
    mDstStageMask |= other->mDstStageMask;
    mMemoryBarrierSrcAccess |= other->mMemoryBarrierSrcAccess;
    mMemoryBarrierDstAccess |= other->mMemoryBarrierDstAccess;
    mImageMemoryBarriers.insert(mImageMemoryBarriers.end(), other->mImageMemoryBarriers.begin(),
                                other->mImageMemoryBarriers.end());
    mBufferMemoryBarriers.insert(mBufferMemoryBarriers.end(), other->mBufferMemoryBarriers.begin(),
                                 other->mBufferMemoryBarriers.end());
    other->reset();
}

void Transition::mergeMemoryBarrier(VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask,
                                    VkFlags srcAccess, VkFlags dstAccess)
{
    mSrcStageMask |= srcStageMask;
    mDstStageMask |= dstStageMask;
    mMemoryBarrierSrcAccess |= srcAccess;
    mMemoryBarrierDstAccess |= dstAccess;
}

void Transition::mergeImageBarrier(VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask,
                                   const VkImageMemoryBarrier& imageMemoryBarrier)
{
    ASSERT(imageMemoryBarrier.pNext == nullptr);
    mSrcStageMask |= srcStageMask;
    mDstStageMask |= dstStageMask;
    mImageMemoryBarriers.push_back(imageMemoryBarrier);
}

void Transition::mergeBufferBarrier(VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask,
                                    const VkBufferMemoryBarrier& bufferMemoryBarrier)
{
    ASSERT(bufferMemoryBarrier.pNext == nullptr);
    mSrcStageMask |= srcStageMask;
    mDstStageMask |= dstStageMask;
    mBufferMemoryBarriers.push_back(bufferMemoryBarrier);
}

void Transition::reset()
{
    mSrcStageMask = 0;
    mDstStageMask = 0;
    mMemoryBarrierSrcAccess = 0;
    mMemoryBarrierDstAccess = 0;
    mImageMemoryBarriers.clear();
    mBufferMemoryBarriers.clear();
}
} // namespace vk