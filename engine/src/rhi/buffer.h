#pragma once
#include "common/memorybuffer.h"
#include "common/util.h"
#include "resources.h"
#include "rhi/defines.h"
#include <cstdlib>
#include <unordered_map>
#include <vector>

namespace rhi
{
class Context;
class Buffer;
class BufferDescriptor;
class ScratchBuffer;
class SubAllocatedBuffer
{
  public:
    SubAllocatedBuffer(ScratchBuffer* buffer, size_t offset, size_t size, DescriptorType type);

    virtual ~SubAllocatedBuffer() = default;

    void terminate(Context* context);

    Result update(Context* context, size_t size, void* data);

    Buffer* getBuffer();

    size_t getOffset();

    size_t getSize();

    BufferDescriptor* getBufferDescriptor();

  protected:
    ScratchBuffer* buffer;
    size_t offset;
    size_t size;
    BufferDescriptor* descriptor;
};

class Buffer
{
  public:
    Buffer(BufferUsageFlags bufferUsage, MemoryPropertyFlags memoryProperty, size_t size);

    virtual ~Buffer() = default;

    virtual Result init(Context* context) = 0;

    virtual void terminate(Context* context) = 0;

    virtual Result update(Context* context, size_t offset, size_t size, void* data) = 0;

    size_t getAlignmentSize();

  protected:
    BufferUsageFlags bufferUsage;
    MemoryPropertyFlags memoryProperty;
    size_t size;
    size_t alignmentSize;
};

class ScratchBuffer
{
  public:
    ScratchBuffer();

    virtual ~ScratchBuffer() = default;

    Result init(Context* context, BufferUsageFlags bufferUsage, MemoryPropertyFlags memoryProperty, size_t size);

    SubAllocatedBuffer* subAllocate(Context* context, size_t size, void* data);

    Result update(Context* context, size_t offset, size_t size, void* data);

    void terminate(Context* context);

    Buffer* getBuffer()
    {
        ASSERT(buffer);
        return buffer;
    }

  protected:
    Buffer* buffer;
    size_t offset;
    DescriptorType type;
    std::vector<SubAllocatedBuffer*> subAllocatedBuffers;
};

} // namespace rhi