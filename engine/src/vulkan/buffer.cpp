#include "buffer.h"
#include "context.h"
#include "device.h"
#include "memory.h"
#include "queue.h"

namespace vk
{
Buffer::Buffer(rhi::BufferUsageFlags bufferUsage, rhi::MemoryPropertyFlags memoryProperty, size_t size)
    : rhi::Buffer(bufferUsage, memoryProperty, size)
{
}

Result Buffer::init(Context* context)
{
    VkBufferCreateInfo bufferCreateInfo = {};
    bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferCreateInfo.pNext = nullptr;
    bufferCreateInfo.queueFamilyIndexCount =
        static_cast<uint32_t>(context->getQueueMap()->getQueueFamilyIndices().size());
    bufferCreateInfo.pQueueFamilyIndices = context->getQueueMap()->getQueueFamilyIndices().data();
    bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    bufferCreateInfo.size = static_cast<VkDeviceSize>(size);
    bufferCreateInfo.usage = static_cast<VkBufferUsageFlags>(bufferUsage);

    vk_try(create(context->getDevice()->getHandle(), bufferCreateInfo));

    const VkMemoryRequirements memoryRequirements = getMemoryRequirements(context->getDevice()->getHandle());

    deviceMemory = new DeviceMemory();
    try(deviceMemory->init(context, memoryRequirements, static_cast<VkMemoryPropertyFlags>(memoryProperty)));

    return Result::Continue;
}

void Buffer::terminate(VkDevice device)
{
    if (valid())
    {
        vkDestroyBuffer(device, mHandle, nullptr);
        mHandle = VK_NULL_HANDLE;
    }

    if (deviceMemory)
    {
        TERMINATE(deviceMemory, device);
    }
}

VkResult Buffer::create(VkDevice device, const VkBufferCreateInfo& createInfo)
{
    ASSERT(!valid());
    return vkCreateBuffer(device, &createInfo, nullptr, &mHandle);
}

const VkMemoryRequirements Buffer::getMemoryRequirements(VkDevice device)
{
    ASSERT(valid());
    VkMemoryRequirements memoryRequirements;
    vkGetBufferMemoryRequirements(device, mHandle, &memoryRequirements);
    return memoryRequirements;
}
} // namespace vk