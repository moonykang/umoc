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
    Buffer(rhi::DescriptorType descriptorType, rhi::BufferUsageFlags bufferUsage,
           rhi::MemoryPropertyFlags memoryProperty, size_t size);

    Result init(rhi::Context* context) override;

    void terminate(rhi::Context* context) override;

    virtual Result update(rhi::Context* context, size_t offset, size_t size, void* data) override;

    VkWriteDescriptorSet getWriteDescriptorSet();

  protected:
    RealBuffer* buffer;
    VkDescriptorBufferInfo bufferInfo;
};

class VertexBuffer : public Buffer
{
  public:
    VertexBuffer(rhi::DescriptorType descriptorType, rhi::BufferUsageFlags bufferUsage,
                 rhi::MemoryPropertyFlags memoryProperty, size_t size);

    virtual void bind(rhi::Context* context, size_t offset) override;
};

class IndexBuffer : public Buffer
{
  public:
    IndexBuffer(rhi::DescriptorType descriptorType, rhi::BufferUsageFlags bufferUsage,
                rhi::MemoryPropertyFlags memoryProperty, size_t size);

    virtual void bind(rhi::Context* context, size_t offset) override;
};

class UniformBuffer : public Buffer
{
  public:
    UniformBuffer(rhi::DescriptorType descriptorType, rhi::BufferUsageFlags bufferUsage,
                  rhi::MemoryPropertyFlags memoryProperty, size_t size);

    virtual void bind(rhi::Context* context, size_t offset) override
    {
    }

    Result update(rhi::Context* context, size_t offset, size_t size, void* data) override final;
};
} // namespace vk