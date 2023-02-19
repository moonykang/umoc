#pragma once

#include "rhi/buffer.h"
#include "vulkan/core.h"

namespace vk
{
class Context;
class DeviceMemory;

// Reference of actual buffer
class Buffer : rhi::Buffer, public WrappedObject<Buffer, VkBuffer>
{
  public:
    Buffer(rhi::BufferUsageFlags bufferUsage, rhi::MemoryPropertyFlags memoryProperty, size_t size);

    virtual Result init(Context* context);

    virtual void terminate(VkDevice device);

  private:
    VkResult create(VkDevice device, const VkBufferCreateInfo& createInfo);

    const VkMemoryRequirements getMemoryRequirements(VkDevice device);

  private:
    DeviceMemory* deviceMemory;
};
} // namespace vk