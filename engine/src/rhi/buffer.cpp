#include "buffer.h"
#include "context.h"
#include "descriptor.h"

namespace rhi
{
const size_t MAX_SUB_BLOCK_SIZE = 4 * 1024 * 1024;
const size_t VERTEX_SCRATCH_BUFFER_SIZE = 512 * 1024 * 1024;
const size_t INDEX_SCRATCH_BUFFER_SIZE = 512 * 1024 * 1024;
const size_t UNIFORM_SCRATCH_BUFFER_SIZE = 256 * 1024 * 1024;
const size_t STORAGE_SCRATCH_BUFFER_SIZE = 256 * 1024 * 1024;

SubAllocatedBuffer* Context::allocateVertexBuffer(size_t size, void* data)
{
    ASSERT(vertexScratchBuffer);
    return vertexScratchBuffer->subAllocate(this, size, data);
}

SubAllocatedBuffer* Context::allocateIndexBuffer(size_t size, void* data)
{
    ASSERT(indexScratchBuffer);
    return indexScratchBuffer->subAllocate(this, size, data);
}

SubAllocatedBuffer* Context::allocateUniformBuffer(size_t size, void* data)
{
    ASSERT(uniformScratchBuffer);
    return uniformScratchBuffer->subAllocate(this, size, data);
}

SubAllocatedBuffer* Context::allocateStorageBuffer(size_t size, void* data)
{
    ASSERT(storageScratchBuffer);
    return storageScratchBuffer->subAllocate(this, size, data);
}

SubAllocatedBuffer::SubAllocatedBuffer(ScratchBuffer* buffer, size_t offset, size_t size, DescriptorType type)
    : buffer(buffer), offset(offset), size(size),
      descriptor(new BufferDescriptor(type, buffer->getBuffer(), offset, size))
{
}

void SubAllocatedBuffer::terminate(Context* context)
{
    if (descriptor)
    {
        delete descriptor;
        descriptor = nullptr;
    }
}

Result SubAllocatedBuffer::update(Context* context, size_t size, void* data)
{
    ASSERT(buffer && this->size >= size);
    buffer->update(context, offset, size, data);
    return Result::Continue;
}

Buffer* SubAllocatedBuffer::getBuffer()
{
    ASSERT(buffer);
    return buffer->getBuffer();
}

size_t SubAllocatedBuffer::getOffset()
{
    return offset;
}

size_t SubAllocatedBuffer::getSize()
{
    return size;
}

BufferDescriptor* SubAllocatedBuffer::getBufferDescriptor()
{
    return descriptor;
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

Result ScratchBuffer::init(Context* context, BufferUsageFlags bufferUsage, MemoryPropertyFlags memoryProperty,
                           size_t size)
{
    buffer = context->allocateBuffer(bufferUsage, memoryProperty, size);

    type = DescriptorType::Uniform_Buffer_Dynamic;
    if ((bufferUsage & BufferUsage::UNIFORM_BUFFER) != 0)
    {
        type = DescriptorType::Uniform_Buffer_Dynamic;
    }
    if ((bufferUsage & BufferUsage::STORAGE_BUFFER) != 0)
    {
        type = DescriptorType::Storage_Buffer_Dynamic;
    }

    return buffer->init(context);
}

SubAllocatedBuffer* ScratchBuffer::subAllocate(Context* context, size_t size, void* data)
{
    SubAllocatedBuffer* subAllocatedBuffer = new SubAllocatedBuffer(this, offset, size, type);
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

Result ScratchBuffer::update(Context* context, size_t offset, size_t size, void* data)
{
    try(buffer->update(context, offset, size, data));

    return Result::Continue;
}
} // namespace rhi