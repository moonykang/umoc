#include "rhi/context.h"
#include "buffer.h"
#include "common/util.h"
#include "rhi/shader.h"
#include "vulkan/context.h"

namespace rhi
{

const size_t MAX_SUB_BLOCK_SIZE = 4 * 1024 * 1024;
const size_t VERTEX_SCRATCH_BUFFER_SIZE = 512 * 1024 * 1024;
const size_t INDEX_SCRATCH_BUFFER_SIZE = 512 * 1024 * 1024;
const size_t UNIFORM_SCRATCH_BUFFER_SIZE = 256 * 1024 * 1024;
const size_t STORAGE_SCRATCH_BUFFER_SIZE = 256 * 1024 * 1024;

Context* Context::createRHIContext(rhi::List rhi)
{
    switch (rhi)
    {
    case rhi::List::Vulkan:
        return new vk::Context();
    default:
        UNREACHABLE();
        return nullptr;
    }
}

Context::Context()
    : platform::Context(), vertexScratchBuffer(nullptr), indexScratchBuffer(nullptr), uniformScratchBuffer(nullptr),
      storageScratchBuffer(nullptr)
{
}

Result Context::initRHI(platform::Window* window)
{
    try(initRHIImplementation(window));

    if (vertexScratchBuffer == nullptr)
    {
        vertexScratchBuffer = new ScratchBuffer();
        vertexScratchBuffer->init(this, BufferUsage::VERTEX_BUFFER | BufferUsage::TRANSFER_DST,
                                  MemoryProperty::DEVICE_LOCAL, VERTEX_SCRATCH_BUFFER_SIZE);
    }

    if (indexScratchBuffer == nullptr)
    {
        indexScratchBuffer = new ScratchBuffer();
        indexScratchBuffer->init(this, BufferUsage::INDEX_BUFFER | BufferUsage::TRANSFER_DST,
                                 MemoryProperty::DEVICE_LOCAL, INDEX_SCRATCH_BUFFER_SIZE);
    }

    if (uniformScratchBuffer == nullptr)
    {
        uniformScratchBuffer = new ScratchBuffer();
        uniformScratchBuffer->init(this, BufferUsage::UNIFORM_BUFFER | BufferUsage::TRANSFER_DST,
                                   MemoryProperty::HOST_COHERENT | MemoryProperty::HOST_VISIBLE,
                                   UNIFORM_SCRATCH_BUFFER_SIZE);
    }

    if (storageScratchBuffer == nullptr)
    {
        storageScratchBuffer = new ScratchBuffer();
        storageScratchBuffer->init(this,
                                   BufferUsage::VERTEX_BUFFER | BufferUsage::STORAGE_BUFFER | BufferUsage::TRANSFER_DST,
                                   MemoryProperty::DEVICE_LOCAL, STORAGE_SCRATCH_BUFFER_SIZE);
    }

    return Result::Continue;
}

void Context::terminateRHI()
{
    waitIdle();

    for (auto shader : shaderMap)
    {
        TERMINATE(shader.second, this);
    }
    shaderMap.clear();

    TERMINATE(vertexScratchBuffer, this);
    TERMINATE(indexScratchBuffer, this);
    TERMINATE(uniformScratchBuffer, this);
    TERMINATE(storageScratchBuffer, this);

    terminateRHIImplementation();
}
} // namespace rhi