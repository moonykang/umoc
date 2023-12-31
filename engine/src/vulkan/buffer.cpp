#include "buffer.h"
#include "commandBuffer.h"
#include "context.h"
#include "device.h"
#include "memory.h"
#include "physicalDevice.h"
#include "queue.h"
#include "resource/image.h"
#include "transition.h"
#include "util.h"

namespace vk
{

rhi::Buffer* Context::allocateBuffer(rhi::BufferUsageFlags bufferUsage, rhi::MemoryPropertyFlags memoryProperty,
                                     size_t size)
{
    if ((bufferUsage & rhi::BufferUsage::STORAGE_BUFFER) != 0)
    {
        return new StorageBuffer(bufferUsage, memoryProperty, size);
    }

    if ((bufferUsage & rhi::BufferUsage::VERTEX_BUFFER) != 0)
    {
        return new VertexBuffer(bufferUsage, memoryProperty, size);
    }

    if ((bufferUsage & rhi::BufferUsage::INDEX_BUFFER) != 0)
    {
        return new IndexBuffer(bufferUsage, memoryProperty, size);
    }

    if ((bufferUsage & rhi::BufferUsage::UNIFORM_BUFFER) != 0)
    {
        return new UniformBuffer(bufferUsage, memoryProperty, size);
    }

    UNREACHABLE();
    return nullptr;
}

Result Context::addTransition(rhi::Buffer* rhiBuffer, size_t offset, size_t size, rhi::ImageLayout src,
                              rhi::ImageLayout dst)
{
    Buffer* buffer = reinterpret_cast<Buffer*>(rhiBuffer);

    const ImageMemoryBarrierData& transitionFrom = kImageMemoryBarrierData[src];
    const ImageMemoryBarrierData& transitionTo = kImageMemoryBarrierData[dst];
    VkPipelineStageFlags srcStageMask = transitionFrom.srcStageMask;
    VkPipelineStageFlags dstStageMask = transitionTo.dstStageMask;
    VkAccessFlags srcAccessMask = transitionFrom.srcAccessMask;
    VkAccessFlags dstAccessMask = transitionFrom.dstAccessMask;

    CommandBuffer* commandBuffer = getActiveCommandBuffer();

    commandBuffer->addTransition(
        new Transition(buffer->getHandle(), offset, size, srcStageMask, dstStageMask, srcAccessMask, dstAccessMask));

    return Result::Continue;
}

RealBuffer::RealBuffer() : deviceMemory(nullptr)
{
}

Result RealBuffer::init(Context* context, rhi::BufferUsageFlags bufferUsage, rhi::MemoryPropertyFlags memoryProperty,
                        size_t size)
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
    try(deviceMemory->init(context, memoryRequirements, memoryProperty));
    vk_try(bindMemory(context->getDevice()->getHandle()));

    return Result::Continue;
}

void RealBuffer::terminate(Context* context)
{
    if (valid())
    {
        context->addGarbage(vk::HandleType::Buffer, mHandle);
        mHandle = VK_NULL_HANDLE;
    }

    TERMINATE(deviceMemory, context);
}

Result RealBuffer::map(Context* context, size_t offset, size_t size, void* mapData)
{
    void* data = nullptr;
    try(deviceMemory->map(context, offset, size, 0, &data));
    memcpy(data, mapData, size);
    deviceMemory->unmap(context);

    return Result::Continue;
}

Result RealBuffer::rmap(Context* context, size_t offset, size_t size, void* mapData)
{
    void* data = nullptr;
    try(deviceMemory->map(context, offset, size, 0, &data));
    memcpy(mapData, data, size);
    deviceMemory->unmap(context);

    return Result::Continue;
}

Result RealBuffer::copy(Context* context, RealBuffer* srcBuffer, VkDeviceSize offset, VkDeviceSize size)
{
    VkBufferCopy copyRegion = {};
    copyRegion.srcOffset = 0;
    copyRegion.dstOffset = offset;
    copyRegion.size = size;

    CommandBuffer* commandBuffer = context->getUploadCommandBuffer();
    commandBuffer->copyBuffer(srcBuffer->getHandle(), mHandle, copyRegion);

    return context->submitUploadCommandBuffer();
}

VkResult RealBuffer::create(VkDevice device, const VkBufferCreateInfo& createInfo)
{
    ASSERT(!valid());
    return vkCreateBuffer(device, &createInfo, nullptr, &mHandle);
}

VkResult RealBuffer::bindMemory(VkDevice device)
{
    ASSERT(valid() && deviceMemory->valid());
    return vkBindBufferMemory(device, mHandle, deviceMemory->getHandle(), 0);
}

const VkMemoryRequirements RealBuffer::getMemoryRequirements(VkDevice device)
{
    ASSERT(valid());
    VkMemoryRequirements memoryRequirements;
    vkGetBufferMemoryRequirements(device, mHandle, &memoryRequirements);
    return memoryRequirements;
}

Buffer::Buffer(rhi::BufferUsageFlags bufferUsage, rhi::MemoryPropertyFlags memoryProperty, size_t size)
    : rhi::Buffer(bufferUsage, memoryProperty, size), bufferInfo()
{
}

Result Buffer::init(rhi::Context* rhiContext)
{
    Context* context = reinterpret_cast<Context*>(rhiContext);

    buffer = new RealBuffer();
    try(buffer->init(context, bufferUsage, memoryProperty, size));

    bufferInfo.buffer = buffer->getHandle();
    bufferInfo.offset = 0;
    bufferInfo.range = VK_WHOLE_SIZE;

    updateAlignmentSize(context);

    return Result::Continue;
}

void Buffer::terminate(rhi::Context* rhiContext)
{
    Context* context = reinterpret_cast<Context*>(rhiContext);

    TERMINATE(buffer, context);
}

Result Buffer::update(rhi::Context* rhiContext, size_t offset, size_t size, void* data)
{
    Context* context = reinterpret_cast<Context*>(rhiContext);

    RealBuffer* stagingBuffer = new RealBuffer();

    try(stagingBuffer->init(context, rhi::BufferUsage::TRANSFER_SRC,
                            rhi::MemoryProperty::HOST_COHERENT | rhi::MemoryProperty::HOST_VISIBLE, this->size));
    try(stagingBuffer->map(context, 0, size, data));
    try(buffer->copy(context, stagingBuffer, offset, size));

    TERMINATE(stagingBuffer, context);

    return Result::Continue;
}

VkBuffer Buffer::getHandle()
{
    return buffer->getHandle();
}

VertexBuffer::VertexBuffer(rhi::BufferUsageFlags bufferUsage, rhi::MemoryPropertyFlags memoryProperty, size_t size)
    : Buffer(bufferUsage, memoryProperty, size)
{
}

void VertexBuffer::bind(rhi::Context* rhiContext, size_t offset)
{
    Context* context = reinterpret_cast<Context*>(rhiContext);
    CommandBuffer* commandBuffer = context->getActiveCommandBuffer();
    commandBuffer->bindVertexBuffers(buffer->getHandle(), offset);
}

IndexBuffer::IndexBuffer(rhi::BufferUsageFlags bufferUsage, rhi::MemoryPropertyFlags memoryProperty, size_t size)
    : Buffer(bufferUsage, memoryProperty, size)
{
}

void IndexBuffer::bind(rhi::Context* rhiContext, size_t offset)
{
    Context* context = reinterpret_cast<Context*>(rhiContext);
    CommandBuffer* commandBuffer = context->getActiveCommandBuffer();
    commandBuffer->bindIndexBuffers(buffer->getHandle(), offset, VK_INDEX_TYPE_UINT32);
}

UniformBuffer::UniformBuffer(rhi::BufferUsageFlags bufferUsage, rhi::MemoryPropertyFlags memoryProperty, size_t size)
    : Buffer(bufferUsage, memoryProperty, size)
{
}

Result UniformBuffer::update(rhi::Context* rhiContext, size_t offset, size_t size, void* data)
{
    Context* context = reinterpret_cast<Context*>(rhiContext);
    try(buffer->map(context, offset, size, data));
    return Result::Continue;
}

StorageBuffer::StorageBuffer(rhi::BufferUsageFlags bufferUsage, rhi::MemoryPropertyFlags memoryProperty, size_t size)
    : Buffer(bufferUsage, memoryProperty, size)
{
}

void VertexBuffer::updateAlignmentSize(Context* context)
{
    alignmentSize = 4;
}

void IndexBuffer::updateAlignmentSize(Context* context)
{
    alignmentSize = 4;
}

void UniformBuffer::updateAlignmentSize(Context* context)
{
    alignmentSize = context->getPhysicalDevice()->getPhysicalDeviceLimits().minUniformBufferOffsetAlignment;
}

void StorageBuffer::updateAlignmentSize(Context* context)
{
    alignmentSize = context->getPhysicalDevice()->getPhysicalDeviceLimits().minStorageBufferOffsetAlignment;
}

void StorageBuffer::bind(rhi::Context* rhiContext, size_t offset)
{
    Context* context = reinterpret_cast<Context*>(rhiContext);
    CommandBuffer* commandBuffer = context->getActiveCommandBuffer();
    commandBuffer->bindVertexBuffers(buffer->getHandle(), offset);
}
} // namespace vk