#pragma once
#include "common/memorybuffer.h"
#include "common/util.h"
#include "rhi/defines.h"

namespace rhi
{

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

  private:
    util::MemoryBuffer memoryBuffer;
    size_t current;
    size_t size;
};

/*
data >
*/

class VertexBuffer
{
  public:
};

class IndexBuffer
{
};

class Buffer
{
  public:
    Buffer(BufferUsageFlags bufferUsage, MemoryPropertyFlags memoryProperty, size_t size)
        : bufferUsage(bufferUsage), memoryProperty(memoryProperty), size(size)
    {
    }

    virtual ~Buffer() = default;

  protected:
    BufferUsageFlags bufferUsage;
    MemoryPropertyFlags memoryProperty;
    size_t size;
};

class ScratchBuffer
{
};

class VertexScratchBuffer : public ScratchBuffer
{
};

class IndexScratchBuffer : public ScratchBuffer
{
};
} // namespace rhi