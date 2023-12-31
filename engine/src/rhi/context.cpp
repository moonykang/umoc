#include "rhi/context.h"
#include "buffer.h"
#include "common/util.h"
#include "rhi/shader.h"
#include "vulkan/context.h"

namespace rhi
{
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
        vertexScratchBuffer = new VertexScratchBuffer();
        vertexScratchBuffer->init(this);
    }

    if (indexScratchBuffer == nullptr)
    {
        indexScratchBuffer = new IndexScratchBuffer();
        indexScratchBuffer->init(this);
    }

    if (uniformScratchBuffer == nullptr)
    {
        uniformScratchBuffer = new UniformScratchBuffer();
        uniformScratchBuffer->init(this);
    }

    if (storageScratchBuffer == nullptr)
    {
        storageScratchBuffer = new StorageScratchBuffer();
        storageScratchBuffer->init(this);
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