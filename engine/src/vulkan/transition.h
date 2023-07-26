#pragma once

#include "core.h"
#include "resources.h"
#include <map>
#include <set>
#include <vector>

namespace vk
{
class Transition
{
  public:
    Transition();

    Transition(rhi::ImageLayout oldLayout, rhi::ImageLayout newLayout, VkImage image,
               VkImageSubresourceRange subresourceRange);

    Transition(VkBuffer buffer, size_t offset, size_t size, VkPipelineStageFlags srcPipelineStage,
               VkPipelineStageFlags dstPipelineStage, VkAccessFlags srcAccess, VkAccessFlags dstAccess);

    ~Transition() = default;

    bool IsShaderReadOnlyLayout(const ImageMemoryBarrierData& imageLayout);

    VkPipelineStageFlags GetImageLayoutSrcStageMask(const ImageMemoryBarrierData& transition);

    VkPipelineStageFlags GetImageLayoutDstStageMask(const ImageMemoryBarrierData& transition);

    bool isEmpty() const
    {
        return mBufferMemoryBarriers.empty() && mImageMemoryBarriers.empty() && mMemoryBarrierDstAccess == 0;
    }

    bool build()
    {
        if (isEmpty())
        {
            return false;
        }

        if (mMemoryBarrierDstAccess != 0)
        {
            VkMemoryBarrier memoryBarrier = {};
            memoryBarrier.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER;
            memoryBarrier.srcAccessMask = mMemoryBarrierSrcAccess;
            memoryBarrier.dstAccessMask = mMemoryBarrierDstAccess;
            memoryBarriers.push_back(memoryBarrier);
        }
        return true;
    }
    // merge two barriers into one
    void merge(Transition* other);

    void mergeMemoryBarrier(VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, VkFlags srcAccess,
                            VkFlags dstAccess);

    void mergeImageBarrier(VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask,
                           const VkImageMemoryBarrier& imageMemoryBarrier);

    void mergeBufferBarrier(VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask,
                            const VkBufferMemoryBarrier& bufferMemoryBarrier);

    void reset();

  public:
    VkPipelineStageFlags getSrcStageMask()
    {
        return mSrcStageMask;
    }
    VkPipelineStageFlags getDstStageMask()
    {
        return mDstStageMask;
    }
    VkFlags getMeoryBarrierSrcAccess()
    {
        return mMemoryBarrierSrcAccess;
    }
    VkFlags getMeoryBarrierDstAccess()
    {
        return mMemoryBarrierDstAccess;
    }
    std::vector<VkMemoryBarrier>* getMemoryBarriers()
    {
        return &memoryBarriers;
    }
    std::vector<VkBufferMemoryBarrier>* getBufferMemoryBarriers()
    {
        return &mBufferMemoryBarriers;
    }
    std::vector<VkImageMemoryBarrier>* getImageMemoryBarriers()
    {
        return &mImageMemoryBarriers;
    }

  private:
    VkPipelineStageFlags mSrcStageMask;
    VkPipelineStageFlags mDstStageMask;
    VkFlags mMemoryBarrierSrcAccess;
    VkFlags mMemoryBarrierDstAccess;
    std::vector<VkMemoryBarrier> memoryBarriers;
    std::vector<VkBufferMemoryBarrier> mBufferMemoryBarriers;
    std::vector<VkImageMemoryBarrier> mImageMemoryBarriers;
};
} // namespace vk