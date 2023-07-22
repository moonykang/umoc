#include "vertexInput.h"
#include "common/util.h"
#include "platform/context.h"
#include "rhi/buffer.h"
#include "rhi/context.h"

namespace model
{
VertexInput::VertexInput() : vertexBuffer(nullptr), indexBuffer(nullptr)
{
}

void VertexInput::terminate(platform::Context* platformContext)
{
    // TERMINATE(vertexBuffer, context->getRHI());
}

Result VertexInput::loadVertexBuffer(platform::Context* platformContext, std::vector<rhi::Vertex>& vertices)
{
    ASSERT(!vertexBuffer);

    rhi::Context* context = reinterpret_cast<rhi::Context*>(platformContext);

    vertexBuffer = context->allocateVertexBuffer(sizeof(rhi::Vertex) * vertices.size(), vertices.data());
    return Result::Continue;
}

Result VertexInput::loadExternalVertexBuffer(platform::Context* context, rhi::StorageBuffer* externalBuffer)
{
    ASSERT(externalBuffer);
    vertexBuffer = reinterpret_cast<rhi::VertexBuffer*>(externalBuffer);
    return Result::Continue;
}

Result VertexInput::loadIndexBuffer(platform::Context* platformContext, std::vector<uint32_t>& indices)
{
    ASSERT(!indexBuffer);

    rhi::Context* context = reinterpret_cast<rhi::Context*>(platformContext);

    indexBuffer = context->allocateIndexBuffer(sizeof(uint32_t) * indices.size(), indices.data());
    return Result::Continue;
}

void VertexInput::bind(platform::Context* platformContext)
{
    rhi::Context* context = reinterpret_cast<rhi::Context*>(platformContext);

    if (vertexBuffer)
    {
        vertexBuffer->bind(context);
    }

    if (indexBuffer)
    {
        indexBuffer->bind(context);
    }
}
} // namespace model