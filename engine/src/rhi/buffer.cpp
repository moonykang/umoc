#include "buffer.h"
#include "context.h"

namespace rhi
{
const size_t VERTEX_SCRATCH_BUFFER_SIZE = 256 * 1024 * 1024;

SubAllocatedBuffer::SubAllocatedBuffer(ScratchBuffer* buffer, size_t offset, size_t size)
    : buffer(buffer), offset(offset), size(size)
{
}

void SubAllocatedBuffer::terminate(Context* context)
{
}

void SubAllocatedBuffer::bind(Context* context)
{
    ASSERT(buffer);
    buffer->bind(context, offset);
}

VertexBuffer::VertexBuffer(ScratchBuffer* buffer, size_t offset, size_t size) : SubAllocatedBuffer(buffer, offset, size)
{
}

IndexBuffer::IndexBuffer(ScratchBuffer* buffer, size_t offset, size_t size) : SubAllocatedBuffer(buffer, offset, size)
{
}

ScratchBuffer::ScratchBuffer() : buffer(nullptr)
{
}

void ScratchBuffer::terminate(Context* context)
{
    for (auto& subAllocatedBuffer : subAllocatedBuffers)
    {
        delete subAllocatedBuffer;
    }
    subAllocatedBuffers.clear();

    TERMINATE(buffer, context);
}

Result VertexScratchBuffer::init(Context* context)
{
    buffer = context->createBuffer(rhi::BufferUsage::VERTEX_BUFFER | rhi::BufferUsage::TRANSFER_DST,
                                   rhi::MemoryProperty::DEVICE_LOCAL, VERTEX_SCRATCH_BUFFER_SIZE);

    try(buffer->init(context));
    return Result::Continue;
}

SubAllocatedBuffer* VertexScratchBuffer::subAllocate(Context* context, size_t size, void* data)
{
    SubAllocatedBuffer* subAllocatedBuffer = new VertexBuffer(this, offset, size);
    subAllocatedBuffers.push_back(subAllocatedBuffer);

    // TODO
    size_t aligned_size = ((size + 3) / 4) * 4;
    if (buffer->allocate(context, offset, size, data) != Result::Continue)
    {
        ASSERT(true);
        return nullptr;
    }

    offset += aligned_size;

    return subAllocatedBuffer;
}

SubAllocatedBuffer* IndexScratchBuffer::subAllocate(Context* context, size_t size, void* data)
{
    SubAllocatedBuffer* subAllocatedBuffer = new IndexBuffer(this, offset, size);
    subAllocatedBuffers.push_back(subAllocatedBuffer);

    // TODO
    size_t aligned_size = ((size + 3) / 4) * 4;
    offset += aligned_size;

    return subAllocatedBuffer;
}

Result IndexScratchBuffer::init(Context* context)
{
    return Result::Continue;
}

} // namespace rhi