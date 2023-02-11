#pragma once
#include "rhi/defines.h"
#include "common/memorybuffer.h"
#include "common/util.h"

namespace rhi
{

template<typename T>
class Buffer
{
public:
    Buffer(size_t size) : size(size), current(0)
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

class VertexBuffer
{
  public:
    VertexBuffer(size_t size);

    ~VertexBuffer() = default;

    Result insert(Vertex& v);

  private:
    Buffer<Vertex>* buffer;
    VertexChannelFlags vertexChannelFlags;
};

class IndexBuffer
{
};
} // namespace rhi