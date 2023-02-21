#include "buffer.h"
#include "commandBuffer.h"
#include "context.h"
#include "device.h"
#include "memory.h"
#include "queue.h"

namespace vk
{

rhi::Buffer* Context::createBuffer(rhi::BufferUsageFlags bufferUsage, rhi::MemoryPropertyFlags memoryProperty,
                                   size_t size)
{
    if ((bufferUsage & rhi::BufferUsage::VERTEX_BUFFER) != 0)
    {
        return new VertexBuffer(bufferUsage, memoryProperty, size);
    }

    return new Buffer(bufferUsage, memoryProperty, size);
}

Buffer::Buffer(rhi::BufferUsageFlags bufferUsage, rhi::MemoryPropertyFlags memoryProperty, size_t size)
    : rhi::Buffer(bufferUsage, memoryProperty, size), stagingBuffer(nullptr)
{
}

Result Buffer::init(rhi::Context* rhiContext)
{
    Context* context = reinterpret_cast<Context*>(rhiContext);

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
    vk_try(bindMemory(context->getDevice()->getHandle()));

    return Result::Continue;
}

void Buffer::terminate(rhi::Context* rhiContext)
{
    Context* context = reinterpret_cast<Context*>(rhiContext);

    if (valid())
    {
        vkDestroyBuffer(context->getDevice()->getHandle(), mHandle, nullptr);
        mHandle = VK_NULL_HANDLE;
    }

    TERMINATE(deviceMemory, context->getDevice()->getHandle());
    TERMINATE(stagingBuffer, rhiContext);
}

Result Buffer::allocate(rhi::Context* rhiContext, size_t offset, size_t size, void* data)
{
    Context* context = reinterpret_cast<Context*>(rhiContext);

    stagingBuffer = new Buffer(rhi::BufferUsage::TRANSFER_SRC,
                               rhi::MemoryProperty::HOST_COHERENT | rhi::MemoryProperty::HOST_VISIBLE, this->size);

    try(stagingBuffer->init(rhiContext));
    try(stagingBuffer->map(context, 0, size, data));
    try(copy(context, stagingBuffer, offset, size));

    return Result::Continue;
}

void Buffer::bind(rhi::Context* rhiContext, size_t offset)
{
    Context* context = reinterpret_cast<Context*>(rhiContext);
    // context->getActiveCommandBuffer()
}

Result Buffer::map(Context* context, size_t offset, size_t size, void* mapData)
{
    void* data = nullptr;
    try(deviceMemory->map(context, offset, size, 0, &data));
    memcpy(data, mapData, size);
    deviceMemory->unmap(context);

    return Result::Continue;
}

Result Buffer::copy(Context* context, Buffer* srcBuffer, VkDeviceSize offset, VkDeviceSize size)
{
    VkBufferCopy copyRegion = {};
    copyRegion.srcOffset = 0;
    copyRegion.dstOffset = offset;
    copyRegion.size = size;

    CommandBuffer* commandBuffer = context->getUploadCommandBuffer();
    commandBuffer->copyBuffer(srcBuffer->getHandle(), mHandle, copyRegion);

    return context->submitUploadCommandBuffer();
}

VkResult Buffer::create(VkDevice device, const VkBufferCreateInfo& createInfo)
{
    ASSERT(!valid());
    return vkCreateBuffer(device, &createInfo, nullptr, &mHandle);
}

VkResult Buffer::bindMemory(VkDevice device)
{
    ASSERT(valid() && deviceMemory->valid());
    return vkBindBufferMemory(device, mHandle, deviceMemory->getHandle(), 0);
}

const VkMemoryRequirements Buffer::getMemoryRequirements(VkDevice device)
{
    ASSERT(valid());
    VkMemoryRequirements memoryRequirements;
    vkGetBufferMemoryRequirements(device, mHandle, &memoryRequirements);
    return memoryRequirements;
}

VertexBuffer::VertexBuffer(rhi::BufferUsageFlags bufferUsage, rhi::MemoryPropertyFlags memoryProperty, size_t size)
    : Buffer(bufferUsage, memoryProperty, size)
{
}

void VertexBuffer::bind(rhi::Context* rhiContext, size_t offset)
{
    Context* context = reinterpret_cast<Context*>(rhiContext);
    CommandBuffer* commandBuffer = context->getActiveCommandBuffer();
    commandBuffer->bindVertexBuffers(mHandle, offset);
}
} // namespace vk