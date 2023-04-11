#include "buffer.h"
#include "context.h"
#include "descriptor.h"

namespace rhi
{
const size_t MAX_SUB_BLOCK_SIZE = 4 * 1024 * 1024;
const size_t VERTEX_SCRATCH_BUFFER_SIZE = 512 * 1024 * 1024;
const size_t INDEX_SCRATCH_BUFFER_SIZE = 512 * 1024 * 1024;
const size_t UNIFORM_SCRATCH_BUFFER_SIZE = 256 * 1024 * 1024;

VertexBuffer* Context::allocateVertexBuffer(size_t size, void* data)
{
    ASSERT(vertexScratchBuffer);
    return reinterpret_cast<rhi::VertexBuffer*>(vertexScratchBuffer->subAllocate(this, size, data));
}

IndexBuffer* Context::allocateIndexBuffer(size_t size, void* data)
{
    ASSERT(indexScratchBuffer);
    return reinterpret_cast<rhi::IndexBuffer*>(indexScratchBuffer->subAllocate(this, size, data));
}

UniformBuffer* Context::allocateUniformBuffer(size_t size, void* data)
{
    ASSERT(uniformScratchBuffer);
    return reinterpret_cast<rhi::UniformBuffer*>(uniformScratchBuffer->subAllocate(this, size, data));
}

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

Result SubAllocatedBuffer::update(Context* context, size_t size, void* data)
{
    ASSERT(buffer && this->size >= size);
    buffer->update(context, offset, size, data);
    return Result::Continue;
}

size_t SubAllocatedBuffer::getOffset()
{
    return offset;
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

BufferDescriptor* UniformBuffer::getBufferDescriptor()
{
    return new BufferDescriptor(rhi::DescriptorType::Uniform_Buffer_Dynamic, buffer->getBuffer(), offset, size);
}

Buffer::Buffer(BufferUsageFlags bufferUsage, MemoryPropertyFlags memoryProperty, size_t size)
    : bufferUsage(bufferUsage), memoryProperty(memoryProperty), size(size), alignmentSize(1)
{
}

size_t Buffer::getAlignmentSize()
{
    return alignmentSize;
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

Result ScratchBuffer::update(Context* context, size_t offset, size_t size, void* data)
{
    try(buffer->update(context, offset, size, data));

    return Result::Continue;
}

Result VertexScratchBuffer::init(Context* context)
{
    buffer = context->allocateBuffer(BufferUsage::VERTEX_BUFFER | BufferUsage::TRANSFER_DST,
                                     MemoryProperty::DEVICE_LOCAL, VERTEX_SCRATCH_BUFFER_SIZE);

    try(buffer->init(context));
    return Result::Continue;
}

SubAllocatedBuffer* VertexScratchBuffer::subAllocate(Context* context, size_t size, void* data)
{
    SubAllocatedBuffer* subAllocatedBuffer = new VertexBuffer(this, offset, size);
    subAllocatedBuffers.push_back(subAllocatedBuffer);

    // TODO
    size_t alignmentSize = buffer->getAlignmentSize();
    size_t aligned_size = ((size + alignmentSize - 1) / alignmentSize) * alignmentSize;

    if (buffer->update(context, offset, size, data) != Result::Continue)
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
    size_t alignmentSize = buffer->getAlignmentSize();
    size_t aligned_size = ((size + alignmentSize - 1) / alignmentSize) * alignmentSize;

    if (buffer->update(context, offset, size, data) != Result::Continue)
    {
        ASSERT(true);
        return nullptr;
    }
    offset += aligned_size;

    return subAllocatedBuffer;
}

Result IndexScratchBuffer::init(Context* context)
{
    buffer = context->allocateBuffer(BufferUsage::INDEX_BUFFER | BufferUsage::TRANSFER_DST,
                                     MemoryProperty::DEVICE_LOCAL, INDEX_SCRATCH_BUFFER_SIZE);

    try(buffer->init(context));
    return Result::Continue;
}

SubAllocatedBuffer* UniformScratchBuffer::subAllocate(Context* context, size_t size, void* data)
{
    SubAllocatedBuffer* subAllocatedBuffer = new UniformBuffer(this, offset, size);
    subAllocatedBuffers.push_back(subAllocatedBuffer);

    // TODO
    size_t alignmentSize = buffer->getAlignmentSize();
    size_t aligned_size = ((size + alignmentSize - 1) / alignmentSize) * alignmentSize;

    if (buffer->update(context, offset, size, data) != Result::Continue)
    {
        ASSERT(true);
        return nullptr;
    }
    offset += aligned_size;

    return subAllocatedBuffer;
}

Result UniformScratchBuffer::init(Context* context)
{
    buffer = context->allocateBuffer(BufferUsage::UNIFORM_BUFFER | BufferUsage::TRANSFER_DST,
                                     MemoryProperty::HOST_COHERENT | MemoryProperty::HOST_VISIBLE,
                                     UNIFORM_SCRATCH_BUFFER_SIZE);

    try(buffer->init(context));
    return Result::Continue;
}

UniformScratchBufferList::UniformScratchBufferList(uint32_t count) : count(count)
{
}
} // namespace rhi