#include "vulkan/commandBuffer.h"
#include "vulkan/sync.h"

namespace vk
{
CommandBuffer::CommandBuffer() : fence(nullptr)
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

    DELETE(fence, device);
}

VkResult CommandBuffer::allocate(VkDevice device, const VkCommandBufferAllocateInfo& allocateInfo)
{
    ASSERT(!valid());
    return vkAllocateCommandBuffers(device, &allocateInfo, &mHandle);
}

Result CommandBuffer::reset(VkDevice device, const bool bWait)
{
    if (fence->getStatus(device) == VK_SUCCESS)
    {
        reset();
        fence->reset(device);

        // delete transition;
        // transition = nullptr;
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

    return vkBeginCommandBuffer(mHandle, &commandBufferBeginInfo);
}
VkResult CommandBuffer::end()
{
    ASSERT(valid());

    // flushTransitions();
    return vkEndCommandBuffer(mHandle);
}

VkFence CommandBuffer::getFence()
{
    ASSERT(fence && fence->valid());
    return fence->getHandle();
}

VkResult CommandBuffer::reset()
{
    ASSERT(valid());
    return vkResetCommandBuffer(mHandle, 0);
}
} // namespace vk