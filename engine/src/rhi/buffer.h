#pragma once
#include "common/memorybuffer.h"
#include "common/util.h"
#include "rhi/defines.h"
#include <unordered_map>
#include <vector>

namespace rhi
{
class Context;

template <typename T> class DataBuffer
{
  public:
    DataBuffer(size_t size) : size(size), current(0)
    {
        const size_t typeSize = sizeof(T) / sizeof(uint8_t);
        ASSERT(typeSize != 0);

        memoryBuffer.resize(size * typeSize);
    }

    Result insert(T& v)
    {
        if (current >= size)
        {
            return Result::Fail;
        }

        current++;
        return Result::Continue;
    }

    size_t getSize()
    {
        return memoryBuffer.size();
    }

    size_t getCount()
    {
        return size;
    }

    void* data()
    {
        return memoryBuffer.data();
    }

  private:
    util::MemoryBuffer memoryBuffer;
    size_t current;
    size_t size;
};

/*
data >
*/

class ScratchBuffer;
class SubAllocatedBuffer
{
  public:
    SubAllocatedBuffer(ScratchBuffer* buffer, size_t offset, size_t size);

    virtual ~SubAllocatedBuffer() = default;

    void terminate(Context* context);

    void bind(Context* context);

  private:
    ScratchBuffer* buffer;
    size_t offset;
    size_t size;
};

class VertexBuffer : public SubAllocatedBuffer
{
  public:
    VertexBuffer(ScratchBuffer* buffer, size_t offset, size_t size);
};

class IndexBuffer : public SubAllocatedBuffer
{
  public:
    IndexBuffer(ScratchBuffer* buffer, size_t offset, size_t size);
};

class Buffer
{
  public:
    Buffer(BufferUsageFlags bufferUsage, MemoryPropertyFlags memoryProperty, size_t size)
        : bufferUsage(bufferUsage), memoryProperty(memoryProperty), size(size)
    {
    }

    virtual ~Buffer() = default;

    virtual Result init(Context* context) = 0;

    virtual void terminate(Context* context) = 0;

    virtual Result allocate(Context* context, size_t offset, size_t size, void* data) = 0;

    virtual void bind(Context* context, size_t offset) = 0;

  protected:
    BufferUsageFlags bufferUsage;
    MemoryPropertyFlags memoryProperty;
    size_t size;
};

class ScratchBuffer
{
  public:
    ScratchBuffer();

    virtual ~ScratchBuffer() = default;

    virtual Result init(Context* context) = 0;

    virtual SubAllocatedBuffer* subAllocate(Context* context, size_t size, void* data) = 0;

    void terminate(Context* context);

    void bind(Context* context, size_t offset)
    {
        ASSERT(buffer);
        buffer->bind(context, offset);
    }

  protected:
    Buffer* buffer;
    size_t offset;
    std::vector<SubAllocatedBuffer*> subAllocatedBuffers;
};

class VertexScratchBuffer : public ScratchBuffer
{
  public:
    Result init(Context* context) override;

    SubAllocatedBuffer* subAllocate(Context* context, size_t size, void* data) override;
};

class IndexScratchBuffer : public ScratchBuffer
{
  public:
    Result init(Context* context) override;

    SubAllocatedBuffer* subAllocate(Context* context, size_t size, void* data) override;
};
} // namespace rhi