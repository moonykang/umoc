#pragma once

#include "rhi/buffer.h"
#include "vulkan/core.h"

namespace vk
{
class Context;
class DeviceMemory;

// Reference of actual buffer
class Buffer : public rhi::Buffer, public WrappedObject<Buffer, VkBuffer>
{
  public:
    Buffer(rhi::BufferUsageFlags bufferUsage, rhi::MemoryPropertyFlags memoryProperty, size_t size);

    Result init(rhi::Context* context) override;

    void terminate(rhi::Context* context) override;

    Result allocate(rhi::Context* context, size_t offset, size_t size, void* data) override;

    virtual void bind(rhi::Context* context, size_t offset) override;

    Result map(Context* context, size_t offset, size_t size, void* mapData);

    Result copy(Context* context, Buffer* srcBuffer, VkDeviceSize offset, VkDeviceSize size);

  private:
    VkResult create(VkDevice device, const VkBufferCreateInfo& createInfo);

    VkResult bindMemory(VkDevice device);

    const VkMemoryRequirements getMemoryRequirements(VkDevice device);

  private:
    DeviceMemory* deviceMemory;
    Buffer* stagingBuffer;
};

class VertexBuffer : public Buffer
{
  public:
    VertexBuffer(rhi::BufferUsageFlags bufferUsage, rhi::MemoryPropertyFlags memoryProperty, size_t size);

    virtual void bind(rhi::Context* context, size_t offset) override;
};

class IndexBuffer : public Buffer
{
  public:
    IndexBuffer(rhi::BufferUsageFlags bufferUsage, rhi::MemoryPropertyFlags memoryProperty, size_t size);

    virtual void bind(rhi::Context* context, size_t offset) override;
};
} // namespace vk