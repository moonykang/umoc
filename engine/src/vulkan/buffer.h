#pragma once

#include "rhi/buffer.h"
#include "vulkan/core.h"

namespace vk
{
class Context;
class DeviceMemory;

class RealBuffer : public WrappedObject<RealBuffer, VkBuffer>
{
  public:
    RealBuffer();

    Result init(Context* context, rhi::BufferUsageFlags bufferUsage, rhi::MemoryPropertyFlags memoryProperty,
                size_t size);

    void terminate(Context* context);

    // Result allocate(rhi::Context* context, size_t offset, size_t size, void* data) override;

    Result map(Context* context, size_t offset, size_t size, void* mapData);

    Result rmap(Context* context, size_t offset, size_t size, void* mapData);

    Result copy(Context* context, RealBuffer* srcBuffer, VkDeviceSize offset, VkDeviceSize size);

  private:
    VkResult create(VkDevice device, const VkBufferCreateInfo& createInfo);

    VkResult bindMemory(VkDevice device);

    const VkMemoryRequirements getMemoryRequirements(VkDevice device);

  private:
    DeviceMemory* deviceMemory;
};

// Reference of actual buffer
class Buffer : public rhi::Buffer
{
  public:
    Buffer(rhi::BufferUsageFlags bufferUsage, rhi::MemoryPropertyFlags memoryProperty, size_t size);

    Result init(rhi::Context* context) override;

    void terminate(rhi::Context* context) override;

    virtual Result update(rhi::Context* context, size_t offset, size_t size, void* data) override;

    virtual void updateAlignmentSize(Context* context) = 0;

    VkBuffer getHandle();

  protected:
    RealBuffer* buffer;
    VkDescriptorBufferInfo bufferInfo;
};

class VertexBuffer : public Buffer
{
  public:
    VertexBuffer(rhi::BufferUsageFlags bufferUsage, rhi::MemoryPropertyFlags memoryProperty, size_t size);

    virtual void bind(rhi::Context* context, size_t offset) override;

    void updateAlignmentSize(Context* context) override;
};

class IndexBuffer : public Buffer
{
  public:
    IndexBuffer(rhi::BufferUsageFlags bufferUsage, rhi::MemoryPropertyFlags memoryProperty, size_t size);

    virtual void bind(rhi::Context* context, size_t offset) override;

    void updateAlignmentSize(Context* context) override;
};

class UniformBuffer : public Buffer
{
  public:
    UniformBuffer(rhi::BufferUsageFlags bufferUsage, rhi::MemoryPropertyFlags memoryProperty, size_t size);

    virtual void bind(rhi::Context* context, size_t offset) override
    {
    }

    Result update(rhi::Context* context, size_t offset, size_t size, void* data) override final;

    void updateAlignmentSize(Context* context) override;
};

class StorageBuffer : public Buffer
{
  public:
    StorageBuffer(rhi::BufferUsageFlags bufferUsage, rhi::MemoryPropertyFlags memoryProperty, size_t size);

    virtual void bind(rhi::Context* context, size_t offset) override;

    void updateAlignmentSize(Context* context) override;
};
} // namespace vk