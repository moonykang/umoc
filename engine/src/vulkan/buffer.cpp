#include "buffer.h"
#include "context.h"
#include "device.h"
#include "memory.h"
#include "queue.h"

namespace vk
{
constexpr uint64_t kVertexScratchBufferSize = 512 * 1024 * 1024;

Buffer::Buffer(const VkBufferUsageFlags usage, const VkMemoryPropertyFlags memoryProperty,
               const VkMemoryAllocateFlags allocateFlags, const size_t size, const size_t offset)
    : deviceMemory(nullptr), usage(usage), memoryProperty(memoryProperty), allocateFlags(allocateFlags), size(size),
      offset(offset)

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
    bufferCreateInfo.usage = usage;

    vk_try(create(context->getDevice()->getHandle(), bufferCreateInfo));

    const VkMemoryRequirements memoryRequirements = getMemoryRequirements(context->getDevice()->getHandle());

    deviceMemory = new DeviceMemory();
    try(deviceMemory->init(context, memoryRequirements, memoryProperty));

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
        DELETE(deviceMemory, device);
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

ScratchBuffer::ScratchBuffer(const VkBufferUsageFlags usage, const VkMemoryPropertyFlags memoryProperty,
                             const VkMemoryAllocateFlags allocateFlags, const size_t size, const size_t offset)
    : Buffer(usage, memoryProperty, allocateFlags, size, offset)
{
}

BufferManager::BufferManager() : vertexScratchBuffer(nullptr)
{
}

Result BufferManager::init(Context* context)
{
    vertexScratchBuffer = new ScratchBuffer(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                                            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 0, kVertexScratchBufferSize, 0);
    try(vertexScratchBuffer->init(context));

    return Result::Continue;
}

void BufferManager::terminate(VkDevice device)
{
    DELETE(vertexScratchBuffer, device);
}

rhi::Buffer* BufferManager::createBuffer(Context* context)
{
    return nullptr;
}
} // namespace vk