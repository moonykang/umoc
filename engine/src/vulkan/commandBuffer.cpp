#include "commandBuffer.h"
#include "sync.h"
#include "transition.h"

namespace vk
{
CommandBuffer::CommandBuffer() : fence(nullptr), transition(nullptr)
{
}

Result CommandBuffer::init(VkDevice device, VkCommandPool commandPool)
{
    VkCommandBufferAllocateInfo commandBufferAllocateInfo = {};
    commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    commandBufferAllocateInfo.pNext = nullptr;
    commandBufferAllocateInfo.commandPool = commandPool;
    commandBufferAllocateInfo.commandBufferCount = 1;
    commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

    vk_try(allocate(device, commandBufferAllocateInfo));

    VkFenceCreateInfo fenceCreateInfo = {};
    fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceCreateInfo.pNext = nullptr;

    fence = new Fence();
    fence->init(device, fenceCreateInfo);

    return Result::Continue;
}

void CommandBuffer::terminate(VkDevice device, VkCommandPool commandPool)
{
    if (valid())
    {
        vkFreeCommandBuffers(device, commandPool, 1, &mHandle);
        mHandle = VK_NULL_HANDLE;
    }

    TERMINATE(fence, device);
}

VkResult CommandBuffer::allocate(VkDevice device, const VkCommandBufferAllocateInfo& allocateInfo)
{
    ASSERT(!valid());
    VkResult result = vkAllocateCommandBuffers(device, &allocateInfo, &mHandle);
    CAPTURE_COMMAND("[vkAllocateCommandBuffers] device: %p, commandBuffer: %p commandPool: %p", device, mHandle,
                    allocateInfo.commandPool);
    return result;
}

Result CommandBuffer::reset(VkDevice device, const bool bWait)
{
    if (fence->getStatus(device) == VK_SUCCESS)
    {
        reset();
        fence->reset(device);

        delete transition;
        transition = nullptr;

        return Result::Continue;
    }
    else if (bWait)
    {
        LOGD("Wait fence");
        vk_try(fence->wait(device, UINT64_MAX));
    }

    return Result::Fail;
}

VkResult CommandBuffer::begin()
{
    ASSERT(valid());

    VkCommandBufferBeginInfo commandBufferBeginInfo = {};
    commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    commandBufferBeginInfo.pNext = nullptr;
    commandBufferBeginInfo.flags = 0;
    commandBufferBeginInfo.pInheritanceInfo = nullptr;

    CAPTURE_COMMAND("[vkBeginCommandBuffer] CB: %p", mHandle);
    return vkBeginCommandBuffer(mHandle, &commandBufferBeginInfo);
}
VkResult CommandBuffer::end()
{
    ASSERT(valid());

    flushTransitions();

    CAPTURE_COMMAND("[vkEndCommandBuffer] CB: %p", mHandle);
    return vkEndCommandBuffer(mHandle);
}

VkFence CommandBuffer::getFence()
{
    ASSERT(fence && fence->valid());
    return fence->getHandle();
}

void CommandBuffer::addTransition(Transition* newTransition)
{
    if (newTransition == nullptr)
    {
        return;
    }

    if (transition == nullptr)
    {
        transition = newTransition;
    }
    else
    {
        transition->merge(newTransition);
    }
}

void CommandBuffer::flushTransitions()
{
    if (transition == nullptr)
    {
        return;
    }

    if (transition->build())
    {
        auto memoryBarriers = transition->getMemoryBarriers();
        auto imageMemoryBarriers = transition->getImageMemoryBarriers();
        auto bufferMemoryBarriers = transition->getBufferMemoryBarriers();

        pipelineBarrier(transition->getSrcStageMask(), transition->getDstStageMask(), 0,
                        static_cast<uint32_t>(memoryBarriers->size()), memoryBarriers->data(),
                        static_cast<uint32_t>(bufferMemoryBarriers->size()), bufferMemoryBarriers->data(),
                        static_cast<uint32_t>(imageMemoryBarriers->size()), imageMemoryBarriers->data());
    }

    transition->reset();
}

VkResult CommandBuffer::reset()
{
    ASSERT(valid());
    CAPTURE_COMMAND("[vkResetCommandBuffer] CB: %p", mHandle);
    return vkResetCommandBuffer(mHandle, 0);
}
} // namespace vk