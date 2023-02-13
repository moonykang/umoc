#include "buffer.h"

namespace rhi
{
VertexBuffer::VertexBuffer(size_t size) : buffer(nullptr), vertexChannelFlags(0)
{
    buffer = new Buffer<Vertex>(size);
}

Result VertexBuffer::insert(Vertex& v)
{

    return Result::Continue;
}
} // namespace rhi