#pragma once
#include "rhi/defines.h"

namespace rhi
{
class Buffer
{
};

class VertexBuffer
{
  public:
    VertexBuffer();

    ~VertexBuffer() = default;

  private:
    Buffer* buffer;
    VertexChannelFlags vertexChannelFlags;
};

class IndexBuffer
{
};
} // namespace rhi