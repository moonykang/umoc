#include "buffer.h"
#include "context.h"

namespace rhi
{
const size_t VERTEX_SCRATCH_BUFFER_SIZE = 256 * 1024 * 1024;
const size_t INDEX_SCRATCH_BUFFER_SIZE = 256 * 1024 * 1024;
const size_t UNIFORM_SCRATCH_BUFFER_SIZE = 256 * 1024 * 1024;

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

Descriptor* SubAllocatedBuffer::getDescriptor()
{
    return buffer->getDescriptor();
}

VertexBuffer::VertexBuffer(ScratchBuffer* buffer, size_t offset, size_t size) : SubAllocatedBuffer(buffer, offset, size)
{
}

IndexBuffer::IndexBuffer(ScratchBuffer* buffer, size_t offset, size_t size) : SubAllocatedBuffer(buffer, offset, size)
{
}

UniformBuffer::UniformBuffer(ScratchBuffer* buffer, size_t offset, size_t size)
    : SubAllocatedBuffer(buffer, offset, size)
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
    buffer =
        context->allocateBuffer(DescriptorType::Uniform_Buffer, BufferUsage::VERTEX_BUFFER | BufferUsage::TRANSFER_DST,
                                MemoryProperty::DEVICE_LOCAL, VERTEX_SCRATCH_BUFFER_SIZE);

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
    if (buffer->allocate(context, offset, size, data) != Result::Continue)
    {
        ASSERT(true);
        return nullptr;
    }
    offset += aligned_size;

    return subAllocatedBuffer;
}

Result IndexScratchBuffer::init(Context* context)
{
    buffer =
        context->allocateBuffer(DescriptorType::Uniform_Buffer, BufferUsage::INDEX_BUFFER | BufferUsage::TRANSFER_DST,
                                MemoryProperty::DEVICE_LOCAL, INDEX_SCRATCH_BUFFER_SIZE);

    try(buffer->init(context));
    return Result::Continue;
}

SubAllocatedBuffer* UniformScratchBuffer::subAllocate(Context* context, size_t size, void* data)
{
    SubAllocatedBuffer* subAllocatedBuffer = new UniformBuffer(this, offset, size);
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

Result UniformScratchBuffer::init(Context* context)
{
    buffer = context->allocateBuffer(
        DescriptorType::Uniform_Buffer_Dynamic, BufferUsage::UNIFORM_BUFFER | BufferUsage::TRANSFER_DST,
        MemoryProperty::HOST_COHERENT | MemoryProperty::HOST_VISIBLE, UNIFORM_SCRATCH_BUFFER_SIZE);

    try(buffer->init(context));
    return Result::Continue;
}

UniformScratchBufferList::UniformScratchBufferList(uint32_t count) : count(count)
{
}
} // namespace rhi